#include "server.h"
#include "ui_server.h"
#include<QMessageBox>
Server::Server(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server),m_totalBytes(0),
    m_bytesReceived(0),m_fileNameSize(0)
{
    ui->setupUi(this);
    connect(&m_tcpServer,SIGNAL(newConnection()),this,SLOT(acceptConnection()));
    connect(ui->btn_listen,SIGNAL(clicked(bool)),this,SLOT(startBtnClicked()));
}



Server::~Server()
{
    delete ui;
}


void Server::start()
{
    if(!m_tcpServer.listen(QHostAddress::Any,ui->line_serverPort->text().toInt()))
    {
        QMessageBox::warning(this,tr("ERROR"),m_tcpServer.errorString());
        close();
        return ;
    }
    ui->btn_listen->setEnabled(false);
    ui->label_server->setText("监听");
    ui->progressBar->reset();
}


void Server::acceptConnection()
{
    m_tcpServerConnection=m_tcpServer.nextPendingConnection();
    connect(m_tcpServerConnection,SIGNAL(readyRead()),this,SLOT(updateServerProgress()));
    connect(m_tcpServerConnection,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
    ui->label_server->setText("接受连接");
    //关闭服务器不再监听，直接进入数据收发
    m_tcpServer.close();
}

void Server::updateServerProgress()
{
    QDataStream in(m_tcpServerConnection);
    in.setVersion(QDataStream::Qt_5_7);
    // 如果接收到的数据小于16个字节，保存到来的文件头结构
    if (m_bytesReceived<=sizeof(qint64)*2)
    {
       if((m_tcpServerConnection->bytesAvailable()>=sizeof(qint64)*2)&&(m_fileNameSize==0))
       {
           // 接收数据总大小信息和文件名大小信息
           in>>m_totalBytes>>m_fileNameSize;
           m_bytesReceived +=sizeof(qint64)*2;
       }
           if((m_tcpServerConnection->bytesAvailable()>=m_fileNameSize)&&(m_fileNameSize!=0))
           {
               // 接收文件名，并建立文件
               in>>m_fileName;
               ui->label_server->setText(tr("接收文件 %1 …").arg(m_fileName));
               m_bytesReceived+=m_fileNameSize;
               m_localFile = new QFile(m_fileName);
               if (!m_localFile->open(QFile::WriteOnly))
               {
                   QMessageBox::warning(this,tr("ERROR"),tr("Server open file error "));
                   return;
               }
           }
           else { return; }
        }
        // 如果接收的数据小于总数据，那么写入文件
        if(m_bytesReceived<m_totalBytes)
        {
           m_bytesReceived+=m_tcpServerConnection->bytesAvailable();
           m_inBlock = m_tcpServerConnection->readAll();
           m_localFile->write(m_inBlock);
           m_inBlock.resize(0);
        }
        ui->progressBar->setMaximum(m_totalBytes);
        ui->progressBar->setValue(m_bytesReceived);

        // 接收数据完成时
        if (m_bytesReceived==m_totalBytes)
        {
           m_tcpServerConnection->close();
           m_localFile->close();
           ui->btn_listen->setEnabled(true);
           ui->label_server->setText(tr("接收文件 %1 成功！").arg(m_fileName));
           QMessageBox::information(this,"通知",m_fileName+"接收成功");
           this->close();
        }
}


void Server::displayError(QAbstractSocket::SocketError socketError)
{
    if(!m_tcpServerConnection->errorString().contains("The remote host closed the connection"))
    {
        Q_UNUSED(socketError)
        QMessageBox::warning(this,tr("ERROR"),m_tcpServerConnection->errorString());
        m_tcpServerConnection->close();
        ui->progressBar->reset();
        ui->label_server->setText("服务端就绪");
        ui->btn_listen->setEnabled(true);
    }
}


void Server::startBtnClicked()
{
    start();
}



























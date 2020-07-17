#include "client.h"
#include "ui_client.h"
#include<QMessageBox>
Client::Client(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Client),m_tcpClient(new QTcpSocket(this)),
     m_totalBytes(0),m_bytesWritten(0),
     m_bytesToWrite(0),m_payloadSize(64*1024)

{
    ui->setupUi(this);
    connect(m_tcpClient,SIGNAL(connected()),this,SLOT(startTransfer()));
    connect(m_tcpClient,SIGNAL(bytesWritten(qint64)),this,SLOT(updateClientProgress(qint64)));
    connect(m_tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
    connect(ui->btn_sendFile,SIGNAL(clicked(bool)),this,SLOT(sendBtnClicked()));
    connect(ui->btn_open,SIGNAL(clicked(bool)),this,SLOT(openBtnClicked()));
}


void Client::openFile()
{
    m_fileName=QFileDialog::getOpenFileName(this);
    if(!m_fileName.isEmpty())
    {
        ui->btn_sendFile->setEnabled(true);
        ui->label_clientstauts->setText(QString("打开文件 %1 成功!").arg(m_fileName));
    }
}

void Client::send()
{
    ui->btn_sendFile->setEnabled(false);
    ui->label_clientstauts->setText("连接中....");
    m_tcpClient->connectToHost(ui->line_ipaddress->text(),ui->line_port->text().toUInt());
}

void Client::startTransfer()
{
    m_localFile=new QFile(m_fileName);
    if(!m_localFile->open(QFile::ReadOnly))
    {
        QMessageBox::warning(this,tr("WARNING"),tr("Client:Open file error!"));
        return ;
    }
    m_totalBytes=m_localFile->size();
    //文件写入到缓冲区
    QDataStream sendout(&m_outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_5_14);
    QString currentFileName=m_fileName.right(m_fileName.size()-m_fileName.lastIndexOf('/')-1);
    //文件信息
    sendout<<qint64(0)<<qint64(0)<<currentFileName;
    m_totalBytes+=m_outBlock.size();
    sendout.device()->seek(0);
    sendout<<m_totalBytes<<qint64(m_outBlock.size()-sizeof (qint64)*2);
    m_bytesToWrite=m_totalBytes- m_tcpClient->write(m_outBlock);
    ui->label_clientstauts->setText("已连接到服务器");
    m_outBlock.resize(0);
}


void Client::updateClientProgress(qint64 numBytes)
{
    m_bytesWritten+=(int)numBytes;
    if(m_bytesToWrite>0)
    {
        m_outBlock=m_localFile->read(qMin(m_bytesToWrite,m_payloadSize));
        m_bytesToWrite-=(int)m_tcpClient->write(m_outBlock);
        m_outBlock.resize(0);
    }
    else
    {
        m_localFile->close();
    }
    ui->progressBar_client->setMaximum(m_totalBytes);
    ui->progressBar_client->setValue(m_bytesWritten);

    if(m_bytesWritten==m_totalBytes)
    {
        ui->label_clientstauts->setText(QString("传送文件 %1 成功").arg(m_fileName));
        m_localFile->close();
        m_tcpClient->close();
        m_outBlock.resize(0);
        ui->label_clientstauts->clear();
        this->close();
    }
}


void Client::displayError(QAbstractSocket::SocketError)
{
    QMessageBox::warning(this,tr("WARNING"),m_tcpClient->errorString());
    m_tcpClient->close();
    ui->progressBar_client->reset();
    ui->label_clientstauts->setText("客户端就绪");
    ui->btn_open->setEnabled(true);
}


void Client::openBtnClicked()
{
    ui->progressBar_client->reset();
    ui->label_clientstauts->setText("状态:等待打开文件");
    openFile();
}


void Client::sendBtnClicked()
{
    send();
}

Client::~Client()
{
    delete ui;
}

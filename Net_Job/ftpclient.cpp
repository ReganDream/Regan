#include "ftpclient.h"
#include "ui_ftpclient.h"
#include<QDebug>
Ftpclient::Ftpclient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Ftpclient)
{
    ui->setupUi(this);
    initDisplay();
    showLocalFile();   
    connect(ui->btn_exit,SIGNAL(clicked()),this,SLOT(close()));
}

Ftpclient::~Ftpclient()
{
    delete ui;
}


void Ftpclient::initDisplay()
{

    ui->btn_cdTo->setEnabled(false);
    ui->btn_upLoad->setEnabled(false);
    ui->btn_downLoad->setEnabled(false);
    ui->progressBar->setValue(0);

    //how the widget shows a context menu
    //用户请求的内容展示
    ui->tree_local->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tree_fileList->setContextMenuPolicy(Qt::CustomContextMenu);
    this->m_server_menu = new QMenu(this);

    //创建删除动作
    QAction *server_rm = new QAction(QObject::tr("删除"),this);
    this->m_server_menu->addAction(server_rm);

}


void Ftpclient::ftpCommandStarted(int )
{
    int id = ftp->currentCommand();
    switch (id)
    {
        case QFtp::ConnectToHost :
            ui->label->setText(tr("正在连接到服务器…"));
            break;
        case QFtp::Login :
            ui->label->setText(tr("正在登录…"));
            break;
        case QFtp::Get :
            ui->label->setText(tr("正在下载…"));
            break;
        case QFtp::Close :
            ui->label->setText(tr("正在关闭连接…"));
    }
}

//ftp操作步骤报错
void Ftpclient::ftpCommandFinished(int , bool error)
{
    if(ftp->currentCommand() == QFtp::ConnectToHost)
    {
        if (error)
        {
            ui->btn_connect->setEnabled(true);
            ui->label->setText(tr("连接服务器出现错误：%1").arg(ftp->errorString()));

        }
        else
        {
            ui->btn_connect->setDisabled(true);
            ui->btn_connect->setText("已连接");
            ui->label->setText(tr("连接到服务器成功"));
        }
    }
    else if (ftp->currentCommand() == QFtp::Login)
    {
        if (error)
        {
            ui->btn_connect->setEnabled(true);
            ui->label->setText(tr("登录出现错误：%1").arg(ftp->errorString()));
            ui->btn_connect->setText("连接");
            QMessageBox::information(this,"通知","连接失败");
        }
        else
        {
            ui->btn_downLoad->setEnabled(true);
            ui->btn_upLoad->setEnabled(true);
            ui->label->setText(tr("登录成功"));
            ftp->list();
        }
    }
    else if (ftp->currentCommand() == QFtp::Get)
    {
        if(error) ui->label->setText(tr("下载出现错误：%1").arg(ftp->errorString()));
        else
        {
            file->close();
            ui->label->setText(tr("下载完成"));
            ui->btn_downLoad->setEnabled(true);
            currentIndex ++;
            if(currentIndex < indexCount)
                this->downloadFtpFile(currentIndex);
            else
            {
                 currentIndex = 0;
                 dirModel->refresh();
            }
        }
        ui->btn_downLoad->setEnabled(true);
    }
    else if (ftp->currentCommand() == QFtp::List)
    {
        if (isDirectory.isEmpty())
        {
            ui->tree_fileList->addTopLevelItem(
                        new QTreeWidgetItem(QStringList()<< tr("<empty>")));
            ui->tree_fileList->setEnabled(false);
            ui->label->setText(tr("该目录为空"));
        }
    }
    else if (ftp->currentCommand() == QFtp::Put)
    {
        if(error) ui->label->setText(tr("上传出现错误：检查文件是否重名！").arg(ftp->errorString()));
        else
        {
            ui->label->setText(tr("上传完成"));
            file->close();
            currentIndex ++;
            if(currentIndex < indexCount)
                this->uploadLocalFile(currentIndex);
            else
            {
                currentIndex = 0;
                //刷新显示列表
                isDirectory.clear();
                ui->tree_fileList->clear();
                ftp->list();
            }
        }
    }
    else if (ftp->currentCommand() == QFtp::Mkdir)
    {
        ui->label->setText(tr("新建文件夹完成"));
        //刷新显示列表
        isDirectory.clear();
        ui->tree_fileList->clear();
        ftp->list();
    }
    else if (ftp->currentCommand() == QFtp::Remove)
    {
        currentIndex++;
        if(currentIndex >= indexCount)
        {
            ui->label->setText(tr("删除完成！"));
            isDirectory.clear();
            ui->tree_fileList->clear();
            ftp->list();
        }
    }
    else if(ftp->currentCommand() == QFtp::Rmdir)
    {
        currentIndex++;
        if(currentIndex >= indexCount)
        {
            ui->label->setText(tr("删除完成！"));
            isDirectory.clear();
            ui->tree_fileList->clear();
            ftp->list();
        }
    }
    else if (ftp->currentCommand() == QFtp::Close)
        ui->label->setText(tr("已经关闭连接"));
    else
        ui->btn_connect->setEnabled(true);
}


void Ftpclient::on_btn_connect_clicked()
{

    ftpAddress = ui->line_ftpServer->text();
    port = ui->line_port->text();
    userName = ui->line_user->text();
    passwd = ui->line_passwd->text();
    if(ftpAddress.isEmpty())
    {
        QMessageBox::information(this,"通知","ip地址不能为空");
        return ;
    }
    ui->tree_fileList->clear();
    currentPath.clear();
    isDirectory.clear();

    ftp = new QFtp(this);
    connect(ftp, SIGNAL(commandStarted(int)), this, SLOT(ftpCommandStarted(int)));
    connect(ftp, SIGNAL(commandFinished(int, bool)),
            this, SLOT(ftpCommandFinished(int, bool)));
    connect(ftp, SIGNAL(listInfo(QUrlInfo)), this, SLOT(addToList(QUrlInfo)));
    connect(ftp, SIGNAL(dataTransferProgress(qint64, qint64)),
            this, SLOT(updateDataTransferProgress(qint64, qint64)));

    connect(ui->tree_fileList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showFtpTreeViewMenu(QPoint)));
    //双击进入目录
    connect(ui->tree_fileList, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
            this, SLOT(processItem(QTreeWidgetItem*, int)));

    //登录
    ftp->connectToHost(ftpAddress,port.toInt());
    ftp->login(userName,passwd);

}


//FtpServer目录
void Ftpclient::addToList(const QUrlInfo &urlInfo)
{

    if(_FromSpecialEncoding(urlInfo.name()).startsWith("."))
        return ;
    QTreeWidgetItem *item = new QTreeWidgetItem;

    item->setText(0, _FromSpecialEncoding(urlInfo.name().toUtf8()));
    qDebug()<<urlInfo.name().toUtf8()<<endl;
    double  dFileSize = ((int)(urlInfo.size()/1024.0*100))/100.0;
    QString fileSize = QString::number(dFileSize,'g',10)+"KB";

    item->setText(1,fileSize);
    if(urlInfo.isDir()) item->setText(2,"文件夹");
    else
    {
        item->setText(2,"");
        item->setText(3,urlInfo.lastModified().toString("yyyy-MM-dd"));
        qDebug()<<urlInfo.lastModified().toString("yyyy-MM-dd")<<endl;
    }


    QPixmap pixmap(urlInfo.isDir() ? ":/png/dir.png" : ":/png/file.png");
    item->setIcon(0,pixmap);
    isDirectory[urlInfo.name()] = urlInfo.isDir();
//    QDateTime timeShow = urlInfo.lastRead();
//    qDebug()<<timeShow.toString("yyyy-MM-dd");
//    item->setText(3,timeShow.toString("yyyy-MM-dd").toUtf8());
    ui->tree_fileList->addTopLevelItem(item);
    if(!ui->tree_fileList->currentItem())
    {
        ui->tree_fileList->setCurrentItem(ui->tree_fileList->topLevelItem(0));
        ui->tree_fileList->setEnabled(true);
    }
}



void Ftpclient::on_tree_fileList_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (item->isDisabled()) return ;
    QString name = item->text(0);
    if (item->text(2)=="文件夹")
    {
        ui->tree_fileList->clear();
        isDirectory.clear();
        currentPath += '/';
        currentPath += name;
        ftp->cd(_ToSpecialEncoding(currentPath));
        ftp->list();  //重新显示文件列表
        ui->btn_cdTo->setEnabled(true);
    }
}


void Ftpclient::processItem(QTreeWidgetItem *item ,int)
{
    if (item->isDisabled()) return ;
    QString name = item->text(0);
    if (isDirectory.value(name))
    {
        ui->tree_fileList->clear();
        isDirectory.clear();
        currentPath += '/';
        currentPath += name;
        ftp->cd(_ToSpecialEncoding(currentPath));
        ftp->list();
        ui->btn_cdTo->setEnabled(true);
    }
}

//返回上一级
void Ftpclient::on_btn_cdTo_clicked()
{
    qDebug()<<"re:currentPath"<<currentPath<<endl;
    ui->tree_fileList->clear();
    isDirectory.clear();
    currentPath = currentPath.left(currentPath.lastIndexOf('/'));
    qDebug()<<"now:currentPath:"<<currentPath<<endl;
    if (currentPath.isEmpty())
    {
        ui->btn_cdTo->setEnabled(false);
        ftp->cd("/");
    }
    else
        ftp->cd(_ToSpecialEncoding(currentPath));
    ftp->list();
}

void Ftpclient::on_btn_upLoad_clicked()
{
    indexCount = ui->tree_local->selectionModel()->selectedRows().count();
    if (indexCount <= 0) return ;
    currentIndex = 0;
    uploadLocalFile(currentIndex);

}


void Ftpclient::uploadLocalFile(int rowIndex)
{
    QModelIndexList  indexList = ui->tree_local->selectionModel()->selectedRows();
    QFileInfo fileInfo = dirModel->fileInfo(indexList.at(rowIndex));
    QString fileName = fileInfo.absoluteFilePath();
    file = new QFile(fileName);
    if(!file->open(QIODevice::ReadOnly))
    {
        delete  file;
        return ;
    }
    ftp->put(file,_ToSpecialEncoding(fileInfo.fileName()));
}



void Ftpclient::on_btn_downLoad_clicked()
{
    if(isDirectory.isEmpty())  return ;

    QModelIndex index = ui->tree_local->currentIndex();
    if(!index.isValid())
    {
        ui->label->setText("请选择正确的下载路径");
        return ;
    }

    indexCount = ui->tree_fileList->selectionModel()->selectedRows().count();
    if (indexCount <= 0) return ;
    ui->btn_downLoad->setEnabled(false);
    currentIndex = 0;
    downloadFtpFile(currentIndex);
}


void Ftpclient::downloadFtpFile(int rowIndex)
{
    QModelIndex index = ui->tree_local->currentIndex();
    QModelIndexList indexList = ui->tree_fileList->selectionModel()->selectedRows();
    QFileInfo fileInfo = dirModel->fileInfo(index);
    QString fileName;
    if(fileInfo.isDir())
        fileName = fileInfo.absoluteFilePath() + indexList.at(rowIndex).data().toString();
    else
        fileName = fileInfo.absoluteFilePath() + "/" +indexList.at(rowIndex).data().toString();
    file = new QFile(fileName);
    if(!file->open(QIODevice::WriteOnly))
    {
        delete  file;
        return ;
    }
    ftp->get(_ToSpecialEncoding(indexList.at(rowIndex).data().toString()),file);

}


void Ftpclient::updateDataTransferProgress(qint64 readBytes,qint64 totalBytes)
{
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(readBytes);
}


void Ftpclient::showLocalFile()
{
    dirModel = new QDirModel;
    dirModel->setReadOnly(false);
    dirModel->setSorting(QDir::DirsFirst | QDir::IgnoreCase | QDir::Name);

    ui->tree_local->setModel(dirModel);
    //扩展视图  设置序列  显示序列
    ui->tree_local->header()->setStretchLastSection(true);
    ui->tree_local->header()->setSortIndicator(0,Qt::AscendingOrder);
    ui->tree_local->header()->setSortIndicatorShown(true);
    ui->tree_local->header()->setEnabled(true);

    QModelIndex index = dirModel->index(QDir::currentPath());
    ui->tree_local->expand(index);
    ui->tree_local->scrollTo(index);
    ui->tree_local->resizeColumnToContents(0);

    QPushButton *btn_Create = new QPushButton(tr("Create Directory..."));
    QPushButton *btn_Del = new QPushButton(tr("Remove"));

    connect(btn_Create,SIGNAL(clicked()),this,SLOT(mkdir()));
    connect(btn_Del,SIGNAL(clicked()),this,SLOT(rm()));
}


void Ftpclient::mkdir()
{
    QModelIndex index = ui->tree_local->currentIndex();
    if (!index.isValid()) return ;
    QString dirName = QInputDialog::getText(this,tr("新建文件夹"),tr("文件夹名称"));
    if (!dirName.isEmpty())
        if(!dirModel->mkdir(index,dirName).isValid())
            QMessageBox::warning(this,tr("警告"),tr("创建文件夹失败"));
}


void Ftpclient::rm()
{
    QModelIndexList indexList = ui->tree_local->selectionModel()->selectedRows();
    if (!indexList.count()) return ;
    bool ok;
    for (int i=0;i<indexList.count();i++)
    {
        QModelIndex index =indexList.at(i);
        if (dirModel->fileInfo(index).isDir())
            ok = dirModel->rmdir(index);
        else
            ok = dirModel->remove(index);
    }
}


void Ftpclient::localDirRefresh()
{
    dirModel->refresh();
}


//有FTP端编码转本地编码
QString Ftpclient::_FromSpecialEncoding(const QString &InputStr)
{
    QString::fromLocal8Bit(InputStr.toLatin1());
    QTextCodec *codec = QTextCodec::codecForName("gbk");
    if (codec)
    {
        return codec->toUnicode(InputStr.toLatin1());
    }
    else
    {
        return QString("");
    }

}


//put get等上传文件时，转换为FTP端编码
QString Ftpclient::_ToSpecialEncoding(const QString &InputStr)
{
    //QString::fromLatin1(InputStr.toLocal8Bit());
    QTextCodec *codec= QTextCodec::codecForName("gbk");
    if (codec)
    {
        return QString::fromLatin1(codec->fromUnicode(InputStr));
    }
    else
    {
        return QString("");
    }
}


void Ftpclient::keyPressEvent(QKeyEvent *event)
{
    //按回车键，默认执行“连接”按钮操作
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        on_btn_connect_clicked();
}



//右键菜单
void Ftpclient::showFtpTreeViewMenu(const QPoint &point)
{
    QMenu *menu = new QMenu;
    menu->addAction(QString(tr("新建文件夹")),this,SLOT(slotMkdir()));
    menu->addAction(QString(tr("刷新")),this,SLOT(slotRefreshFtpList()));
    menu->addAction(QString(tr("删除")),this,SLOT(slotDeleteFile()));
    menu->exec(QCursor::pos());
}


//新建文件夹
void Ftpclient::slotMkdir()
{
    QString dirName = QInputDialog::getText(this,tr("新建文件夹"),tr("文件夹名称"));
    if(!dirName.isEmpty())
        ftp->mkdir(_ToSpecialEncoding(dirName));
}


//刷新
void Ftpclient::slotRefreshFtpList()
{
    isDirectory.clear();
    ui->tree_fileList->clear();
    ftp->list();
}



//删除文件
void Ftpclient::slotDeleteFile()
{
    QModelIndexList indexList = ui->tree_fileList->selectionModel()->selectedRows();
    QString fileName;
    currentIndex = 0;
    indexCount = indexList.count();
    for (int i=0;i<indexCount;i++)
    {
        fileName = indexList.at(i).data().toString();
        if (isDirectory.value(fileName))
            ftp->rmdir(_ToSpecialEncoding(fileName));
        else
            ftp->remove(_ToSpecialEncoding(fileName));
    }

}

//本地文件 右键操作
void Ftpclient::showLocalTreeViewMenu(const QPoint &pos)
{
    QMenu *menu = new QMenu;
    menu->addAction(QString(tr("新建文件夹")),this,SLOT(slotMkdir()));
    menu->addAction(QString(tr("刷新")),this,SLOT(slotRefreshFtpList()));
    menu->addAction(QString(tr("删除")),this,SLOT(slotDeleteFile()));
    menu->exec(QCursor::pos());

}


























































#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QTextCodec>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,email(new Emailsend)
    ,login(new Email_Login)
    ,ftpClient(new Ftpclient)
{
    ui->setupUi(this);
    connect(ui->btn_server,SIGNAL(clicked()),this,SLOT(server_Show()));
    connect(ui->btn_client,SIGNAL(clicked()),this,SLOT(client_Show()));
    connect(ui->btn_ftp,SIGNAL(clicked()),ftpClient,SLOT(show()));
    connect(ui->btn_sendMail,SIGNAL(clicked()),login,SLOT(show()));
    connect(ui->action_exit,SIGNAL(triggered()),this,SLOT(close()));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}

void MainWindow::client_Show()
{
    client =new Client;
    client->show();
}

void MainWindow::server_Show()
{
    server = new Server;
    server->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}


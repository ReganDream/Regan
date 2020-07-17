#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<server.h>
#include<client.h>
#include<emailsend.h>
#include<ftpserver.h>
#include"email_login.h"
#include"ftpclient.h"
#include<QDebug>
#include<QObject>
#include<QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void client_Show();
    void server_Show();
private:
    Ui::MainWindow *ui;
    Server *server;
    Client *client;
    Ftpserver *ftp;
    Email_Login *login;
    Emailsend *email;
    Ftpclient *ftpClient;
};
#endif // MAINWINDOW_H

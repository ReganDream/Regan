#ifndef EMAIL_LOGIN_H
#define EMAIL_LOGIN_H

#include <QWidget>
#include<QMessageBox>
#include<QObject>
#include<QTcpSocket>
#include"emailsend.h"
namespace Ui {
class Email_Login;
}

class Email_Login : public QWidget
{
    Q_OBJECT

public:
    explicit Email_Login(QWidget *parent = nullptr);
    ~Email_Login();

private slots:
    void checkConnectState();
    void readyReadSlot();
    void start();
    void sendMailFrom();

private:
    void checkLogin();
    void sendUser();
    void sendPassword();
    void displayError(QString errpr);

    void sendRecpTo();
    void sendData();
    void sendContent();
    void isSuccess();
    void isSendSuccess();

private:
    Ui::Email_Login *ui;
    bool isSendMail ;
    class Emailsend *email;
    QString expectedReply;
    QString commandLine;
    QString receiveMail;
    QString title;
    QString content;
    void (Email_Login::*nextAction) ();  //收到正确应答后下一步要执行的方法
};

#endif // EMAIL_LOGIN_H

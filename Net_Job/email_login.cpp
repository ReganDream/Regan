#include "email_login.h"
#include "ui_email_login.h"
#include<QMessageBox>
#include"global.h"
Email_Login::Email_Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Email_Login)
{
    ui->setupUi(this);

    isSendMail = false;
    senderMail = ui->line_emailAddress->text();
    email = new Emailsend;
    senderMail = ui->line_emailAddress->text();
    connect(ui->btn_login,SIGNAL(clicked()),this,SLOT(start()));
    connect(ui->btn_exit,SIGNAL(clicked()),this,SLOT(close()));
    connect(email,SIGNAL(sendMail()),this,SLOT(sendMailFrom()));
}


void Email_Login::start()
{

    senderMail = ui->line_emailAddress->text();
    if(senderMail.isEmpty())
    {
        QMessageBox::information(this,"通知","用户名为空");
        return ;
    }

    ui->label_state->setText(QString("正在连接服务器......"));
    ui->btn_login->setEnabled(false);
    authCode = ui->line_passwd->text();
    socketSendMail = new QTcpSocket;
    socketSendMail->connectToHost("smtp.163.com",25);
    this->expectedReply = QString("220 163.com Anti-spam GT for Coremail System");
    this->nextAction = &Email_Login::checkConnectState;
    connect(socketSendMail,SIGNAL(readyRead()),this,SLOT(readyReadSlot()));

}


void Email_Login::displayError(QString error)
{
    QMessageBox::warning(this,tr("通知"),error);
    //ui->label_state->setText(error);
    ui->btn_login->setEnabled(true);
}


void Email_Login::readyReadSlot()
{
    QByteArray buffer = socketSendMail->readAll();
    ui->label_state->setText(buffer);
    qDebug()<<expectedReply<<endl;
    if (buffer.contains(this->expectedReply.toLatin1()))
      {
        if (nextAction) (this->*nextAction) ();
      }
    else
    {
        if (isSendMail)
            QMessageBox::warning(this, "提示", "发送邮件失败");
        else
            QMessageBox::warning(this, "提示", "登录失败");
    }
}


void Email_Login::checkConnectState()
{
   if (socketSendMail->state() == QAbstractSocket::ConnectedState)
   {
       commandLine = QString("helo 163.com\r\n");
       socketSendMail->write(commandLine.toLatin1());
       this->expectedReply = "250 OK\r\n";
       ui->label_state->setText(expectedReply);
       this->nextAction = &Email_Login::checkLogin;
   }
   else
       displayError("连接失败");
}


void Email_Login::checkLogin()
{
    commandLine = QString("auth login\r\n");
    socketSendMail->write(commandLine.toLatin1());
    this->expectedReply = "334 dXNlcm5hbWU6\r\n";
    ui->label_state->setText(expectedReply);
    this->nextAction = &Email_Login::sendUser;
}


void Email_Login::sendUser()
{
    commandLine = QString("\r\n").prepend(senderMail.toLatin1().toBase64());
    socketSendMail->write(commandLine.toLatin1());
    this->expectedReply = "334 UGFzc3dvcmQ6\r\n";
    ui->label_state->setText(expectedReply);
    this->nextAction = &Email_Login::sendPassword;
}


void Email_Login::sendPassword()
{
    commandLine = QString("\r\n").prepend(authCode.toLatin1().toBase64());
    socketSendMail->write(commandLine.toLatin1());
    this->expectedReply = "235 Authentication successful\r\n";
    ui->label_state->setText(expectedReply);
    this->nextAction = &Email_Login::isSuccess;
}

void Email_Login::sendMailFrom()
{
    this->receiveMail = email->receiveMail;
    this->title = email->title;
    this->content = email->content;
    isSendMail = true;
    commandLine = QString("mail from: <%1>\r\n").arg(senderMail);
    socketSendMail->write(commandLine.toLatin1());
    this->expectedReply=QString("250 Mail OK\r\n");
    this->nextAction = &Email_Login::sendRecpTo;
}


void Email_Login::sendRecpTo()
{
    commandLine = QString("rcpt to: <%1>\r\n").arg(receiveMail);
    socketSendMail->write(commandLine.toLatin1());
    this->expectedReply=QString("250 Mail OK\r\n");
    this->nextAction = &Email_Login::sendData;
}

void Email_Login::sendData()
{
    commandLine = QString("data\r\n");
    socketSendMail->write(commandLine.toLatin1());
    this->expectedReply=QString("354 End data with <CR><LF>.<CR><LF>\r\n");
    this->nextAction = &Email_Login::sendContent;
}


void Email_Login::sendContent()
{
    QString content_total = QString("from:%1\r\n"
                                "to:%2\r\n"
                                "subject:=?UTF-8?B?%3?=\r\n"
                                "\r\n"
                                "%4\r\n"
                                "\r\n"
                                ".\r\n").arg(senderMail).arg(receiveMail).arg(QString("").append(title.toUtf8().toBase64())).arg(content);
    socketSendMail->write(content_total.toUtf8());
    this->expectedReply = QString("250 Mail OK queued as");
    this->nextAction = &Email_Login::isSendSuccess;

}

void Email_Login::isSuccess()
{
    QMessageBox::information(this, tr("提示"), tr("服务器连接成功"));
    email->show();
    this->close();
}

void Email_Login::isSendSuccess()
{
    QMessageBox::information(this, tr("提示"), tr("发送邮件成功"));
    email->close();
}

Email_Login::~Email_Login()
{
    delete ui;
}

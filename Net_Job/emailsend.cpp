#include "emailsend.h"
#include "ui_emailsend.h"
#include"global.h"
Emailsend::Emailsend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Emailsend)
{
    ui->setupUi(this);
    ui->line_myAddress->setText(senderMail);
    connect(ui->btn_send,SIGNAL(clicked()),this,SLOT(sendLogin()));
    connect(ui->btn_exit,SIGNAL(clicked()),this,SLOT(close()));

}

void Emailsend::sendLogin()
{

    receiveMail = ui->line_recvAddress->text();
    title = ui->line_subject->text();
    content = ui->text_content->toPlainText();
    emit sendMail();
}


Emailsend::~Emailsend()
{
    delete ui;
}

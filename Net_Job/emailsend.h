#ifndef EMAILSEND_H
#define EMAILSEND_H

#include <QWidget>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QMessageBox>
#include <QDebug>
#include"email_login.h"
namespace Ui {
class Emailsend;
}

class Emailsend : public QWidget
{
    Q_OBJECT

public:
    explicit Emailsend(QWidget *parent = nullptr);
    ~Emailsend();

    QString receiveMail;
    QString title;
    QString content;
    QString expectedReply;

signals:
    void sendMail();

private slots:
    void sendLogin();

private:
    Ui::Emailsend *ui;

};

#endif // EMAILSEND_H

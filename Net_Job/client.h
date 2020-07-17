#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include<QAbstractSocket> //sock编程
#include<QTcpSocket>
#include<QFile>
#include<QtNetwork>
#include<QFileDialog>
namespace Ui {
class Client;
}

class Client : public QWidget
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr);
    ~Client();

public slots:
    void openFile();
    void send();
    void startTransfer();
    void updateClientProgress(qint64);
    void displayError(QAbstractSocket::SocketError);
    void openBtnClicked();
    void sendBtnClicked();

private:
    Ui::Client *ui;
    QTcpSocket *m_tcpClient;
    QFile *m_localFile;
    qint64 m_totalBytes;
    qint64 m_bytesWritten;
    qint64 m_bytesToWrite;
    qint64 m_payloadSize;
    QString m_fileName;
    QByteArray m_outBlock;
};

#endif // CLIENT_H

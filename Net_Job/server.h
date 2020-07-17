#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QAbstractSocket>
#include <QTcpServer>
#include<QTcpSocket>
#include<QFile>
namespace Ui {
class Server;
}

class Server : public QWidget
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = nullptr);
    ~Server();


public slots:
    void start();
    void acceptConnection();
    void updateServerProgress();
    void displayError(QAbstractSocket::SocketError socketError);
    void startBtnClicked();


private:
    Ui::Server *ui;

    QTcpServer m_tcpServer;
    QTcpSocket *m_tcpServerConnection;
    qint64 m_totalBytes;
    qint64 m_bytesReceived;
    qint64 m_fileNameSize;
    QString m_fileName;
    QFile *m_localFile;
    QByteArray m_inBlock;
};

#endif // SERVER_H

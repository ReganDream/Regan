#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QWidget>
#include <QDir>
#include <QDirModel>
#include <QTreeView>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <QKeyEvent>
#include<QFtp>
#include<QFile>
#include<QTreeWidgetItem>
#include<QHash>

namespace Ui {
class Ftpclient;
}

class Ftpclient : public QWidget
{
    Q_OBJECT

public:
    explicit Ftpclient(QWidget *parent = nullptr);
    ~Ftpclient();

private:
    Ui::Ftpclient *ui;
    QString ftpAddress;
    QString userName;
    QString port;
    QString passwd;
    QFtp *ftp;
    QFile *file;

    //初始化界面显示
    void initDisplay();
    // 用来存储FTP端一个路径是否为目录的信息
    QHash<QString, bool> isDirectory;
    //判断本地路径是否为目录
    QHash<QString, bool> isLocalDirectory;
    // 用来存储现在的路径
    QString currentPath;
    QDirModel *dirModel;
    //用来表示下载的文件

    void showLocalFile();
    //两个转码函数，解决中文名文件乱码
    QString _ToSpecialEncoding(const QString &InputStr);
    QString _FromSpecialEncoding(const QString &InputStr);
    //下载FTP端文件
    void downloadFtpFile(int rowIndex);
    void uploadLocalFile(int rowIndex);
    //客户端，服务器端treeview右键菜单
    QMenu *m_server_menu;
    QMenu *m_client_menu;

    //记录上传和下载文件的个数
    int indexCount;
    int currentIndex;

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void ftpCommandStarted(int);
    void ftpCommandFinished(int, bool);

    // 更新进度条
   void updateDataTransferProgress(qint64, qint64 );
    // 将服务器上的文件添加到Tree Widget部件中
    void addToList(const QUrlInfo &urlInfo);
    // 双击一个目录时显示其内容
   void on_btn_connect_clicked();
   void processItem(QTreeWidgetItem*, int);
    //新建目录与删除操作
   void mkdir();
   void rm();
   void localDirRefresh();
    //TreeView显示右键菜单
    void showFtpTreeViewMenu(const QPoint &point );
    void showLocalTreeViewMenu(const QPoint &point );

    void slotMkdir();
    void slotDeleteFile();
    void slotRefreshFtpList();


   void on_tree_fileList_itemDoubleClicked(QTreeWidgetItem *item, int column);
   void on_btn_cdTo_clicked();
   void on_btn_downLoad_clicked();
   void on_btn_upLoad_clicked();
};

#endif // FTPCLIENT_H

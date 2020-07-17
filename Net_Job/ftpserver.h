#ifndef FTPSERVER_H
#define FTPSERVER_H

#include <QWidget>

namespace Ui {
class Ftpserver;
}

class Ftpserver : public QWidget
{
    Q_OBJECT

public:
    explicit Ftpserver(QWidget *parent = nullptr);
    ~Ftpserver();

private:
    Ui::Ftpserver *ui;
};

#endif // FTPSERVER_H

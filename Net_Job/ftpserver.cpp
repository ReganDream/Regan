#include "ftpserver.h"
#include "ui_ftpserver.h"

Ftpserver::Ftpserver(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Ftpserver)
{
    ui->setupUi(this);
}

Ftpserver::~Ftpserver()
{
    delete ui;
}

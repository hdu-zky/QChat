#include "autoLogin.h"
#include "ui_autologin.h"
#include <QMovie>

autoLogin::autoLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::autoLogin)
{
    ui->setupUi(this);
    QMovie *movie = new QMovie(this);
    movie->setFileName(":/images/login");
    ui->movie->setMovie(movie);
    movie->start();
}

autoLogin::~autoLogin()
{
    delete ui;
}

void autoLogin::on_cancel_clicked()
{
    this->close();
}

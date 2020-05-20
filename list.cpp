#include "list.h"
#include "ui_list.h"
#include <QMovie>

List::List(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::List)
{
    ui->setupUi(this);
    QMovie *movie = new QMovie(this);
    movie->setFileName(":/images/login");
    ui->movie->setMovie(movie);
    movie->start();
    }

List::~List()
{
    delete ui;
}

void List::on_cancel_clicked()
{
    this->close();
}

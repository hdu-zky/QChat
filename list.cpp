#include "list.h"
#include "ui_list.h"

List::List(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::List)
{
    ui->setupUi(this);
}

List::~List()
{
    delete ui;
}

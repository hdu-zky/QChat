#include "filetrans.h"
#include "ui_filetrans.h"

fileTrans::fileTrans(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fileTrans)
{
    ui->setupUi(this);
}

fileTrans::~fileTrans()
{
    delete ui;
}

void fileTrans::on_openFile_clicked()
{

}

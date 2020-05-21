#include "chat.h"
#include "ui_dialogwindow.h"
#include <QListWidget>
#include <QDateTime>
DialogWindow::DialogWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWindow)
{
    ui->setupUi(this);
    this->setFixedSize(573,545);
    ui->send->setEnabled(false);
    connect(ui->textEdit,SIGNAL(textChanged()),this,SLOT(enableSendBtn()));
    session = new QStandardItemModel(this);
    session->setHorizontalHeaderItem(0,new QStandardItem(QObject::tr("聊天记录")));
    // 自动延伸填满整个宽度
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->hide();
    // 背景色交替
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setStyleSheet("QTableView::item {alternate-background-color: #f3f4f5;}");
    // 设置选中行背景色
    ui->tableView->setStyleSheet("QTableView::item:selected {background-color: #F8F0DD;color: #3a3a3a;}");
    //设置不显示格子线
    ui->tableView->setShowGrid(false);
    ui->tableView->setModel(session);
    rawCount =0;
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
}

DialogWindow::~DialogWindow()
{
    delete ui;
}

void DialogWindow::enableSendBtn(){
    if(!ui->textEdit->toPlainText().isEmpty()){
        ui->send->setEnabled(true);
    }else{
        ui->send->setEnabled(false);
    }
}

void DialogWindow::on_send_clicked()
{
    QString word = ui->textEdit->toPlainText();
    QString time = QDateTime::currentDateTime().toString(QString("小李 ") +"yyyy-MM-dd hh:mm:ss");
    session->setItem(rawCount++, new QStandardItem(QIcon(":/images/me"), time));
    session->setItem(rawCount++, new QStandardItem(word));
    // 根据内容自动调整行高
    ui->tableView->resizeRowToContents(rawCount-1);
    ui->textEdit->clear();
    ui->send->setEnabled(false);
}

void DialogWindow::on_close_clicked()
{
    this->close();
}

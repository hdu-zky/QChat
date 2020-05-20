#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogwindow.h"
#include "list.h"
#include "user.h"
#include "login.h"

#include <QListWidget>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QtCore/QCoreApplication>

QSqlDatabase getDB(){
    QSqlDatabase dataBase;
    dataBase=QSqlDatabase::addDatabase("QMYSQL");
    dataBase.setHostName("localhost");
    dataBase.setUserName("root");
    dataBase.setPassword("223412");
    dataBase.setDatabaseName("test");
    dataBase.open();
    return dataBase;
}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainToolBar->setEnabled(false);
    ui->menuBar->setEnabled(false);
    ui->statusBar->setEnabled(false);
    ui->mainToolBar->setVisible(false);
    ui->menuBar->setVisible(false);
    ui->statusBar->setVisible(false);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //单击选择一行
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection); //设置只能选择一行，不能多行选中
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);   //设置每行内容不可更改
    ui->tableWidget->setAlternatingRowColors(true);                        //设置隔一行变一颜色，即：一灰一白
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_pushButton_clicked()
{
    list = new List(this);
    list->show();
}

void MainWindow::on_addFriendGroup_clicked()
{

}

void MainWindow::on_openDialogWindow_clicked()
{
    dw = new DialogWindow(this);
    if(ui->tableWidget->selectedItems().isEmpty()){
        QMessageBox::warning(0,QString("警告"),QString("你没有选择群聊或好友！"),QMessageBox::Ok);
        return;
    }else{
        QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
        QString name = items.at(1)->text();
        dw->setWindowTitle(name);
        dw->show();
    }
}

void MainWindow::on_refresh_clicked()
{
    ui->tableWidget->clear();
    QSqlQuery query(getDB());
    if(userId.isEmpty()){
        QMessageBox::warning(0,QString("警告"),QString("账号异常，请重新登录！"),QMessageBox::Ok);
        close();
    }
    // 使数据库支持中文
    QString sql = QString("select uid, userName from user where uid in(select fr_uid from friend where me_uid = '%1')").arg(userId);
    // 使数据库支持中文
    query.exec(sql);
    QString uid , uName;
    int i=0;
    while(query.next()){
        qDebug()<<query.value(0).toInt()<<query.value(1).toString()<<endl;
        uid = query.value(0).toString();
        uName = query.value(1).toString();
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QIcon(":/images/me"), uid));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(uName));
        i++;
    }
    query.finish();
    query.clear();
    getDB().close();
}
// 实现退出重新登录
void MainWindow::on_quit_clicked()
{
    QSettings settings("WWB-Qt","QChat"); //注册表键组
    settings.setValue("autod", 0); // 关闭自动登陆
    qApp->exit(2);
}

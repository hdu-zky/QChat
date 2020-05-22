#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chat.h"
#include "autoLogin.h"
#include "login.h"
#include "findgroup.h"
#include "updateinfo.h"
#include "creategroup.h"

#include <QListWidget>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QtCore/QCoreApplication>
#include<QAction>

QString UId="", UName="";
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
    this->setFixedSize(400,544);
    ui->mainToolBar->setEnabled(false);
//    ui->menuBar->setEnabled(false);
    ui->statusBar->setEnabled(false);
    ui->mainToolBar->setVisible(false);
//    ui->menuBar->setVisible(false);
    ui->statusBar->setVisible(false);

    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //单击选择一行
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection); //设置只能选择一行，不能多行选中
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);   //设置每行内容不可更改
    ui->tableWidget->setAlternatingRowColors(true);                        //设置隔一行变一颜色，即：一灰一白
    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");
    ui->tableWidget->setShowGrid(false); //不显示格子线
    // 设置选中行背景色
    ui->tableWidget->setStyleSheet("QTableWidget::item:selected {background-color: #F8F0DD;color: #3a3a3a;}");
    //设置无边框
    ui->tableWidget->setFrameShape(QFrame::NoFrame);
//    ui->tableWidget->horizontalScrollBar().setStyleSheet("QScrollBar{background:transparent; height:10px;}"
//                                                       "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
//                                                       "QScrollBar::handle:hover{background:gray;}"
//                                                       "QScrollBar::sub-line{background:transparent;}"
//                                                       "QScrollBar::add-line{background:transparent;}");
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu); //开启右键菜单
    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customMenupPop(QPoint)));
//    connect(ui->tableWidget, SIGNAL(clicked(QModelIndex)),SLOT(on_openDialogWindow_clicked()));
    connect(ui->exit,SIGNAL(triggered(bool)),this,SLOT(close()));
    connect(ui->updateInfo,SIGNAL(triggered(bool)),this,SLOT(updateInf()));
    connect(ui->createGroup,SIGNAL(triggered(bool)),this,SLOT(createGp()));
    connect(ui->reSign,SIGNAL(triggered(bool)),this,SLOT(on_quit_clicked()));
    dlgCount =0;
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::customMenupPop(QPoint pos){
//    QTableWidgetItem* selecteditem = ui->tableWidget->itemAt(pos) ;
//    QString finalStr = selecteditem->text();
    QMenu* menu = new QMenu(this);
    QAction *action = new QAction(tr("&Open Directory"), this);
//    action->setData(finalStr);
    menu->addAction(action);
    menu->popup(ui->tableWidget->viewport()->mapToGlobal(pos));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(updateGpInf));
}
void MainWindow::updateGpInf(){
    qDebug()<<"updateGpInf"<<endl;
}

void MainWindow::on_addFriendGroup_clicked()
{
    findGroup *fg = new findGroup(this);
    fg->setUId(userId);
    fg->show();
    on_refresh_clicked();
}

void MainWindow::on_openDialogWindow_clicked()
{
    if(dlgCount == 0){
        // 放到构造函数中，防止每次和一个新的好友聊天都创建一个新的对象
        chatStack = new chatDlgStack(this);
        dlgCount++;
    }
    if(ui->tableWidget->selectedItems().isEmpty()){
        QMessageBox::warning(0,QString("警告"),QString("你没有选择群聊或好友！"),QMessageBox::Ok);
        return;
    }else{
        QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
        QString uid = items.at(1)->text();
        QString name = items.at(2)->text();
        qDebug()<<"\non_openDialogWindow_clicked"<<uid<<name<<endl;
        chatStack->addChatDlg(uid, name);
        chatStack->show();
        qDebug()<<"\nshow"<<endl;
    }
//    dw = new chat(this);
//    if(ui->tableWidget->selectedItems().isEmpty()){
//        QMessageBox::warning(0,QString("警告"),QString("你没有选择群聊或好友！"),QMessageBox::Ok);
//        return;
//    }else{
//        QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
//        QString uid = items.at(1)->text();
//        QString name = items.at(2)->text(), title = name + QString("(%1)").arg(uid);
//        dw->setUId(uid); dw->setUserName(name); //将选中的用户名和id复制到chat中
//        dw->setWindowTitle(title);
//        dw->show();
//    }
}

void MainWindow::on_refresh_clicked()
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->clearContents();
    QSqlQuery query(getDB());
    if(userId.isEmpty()){
        QMessageBox::warning(0,QString("警告"),QString("账号异常，请重新登录！"),QMessageBox::Ok);
        close();
    }
    // 使数据库支持中文
    QString sql = QString("select uid, userName from user where uid in(select fr_uid from friend where me_uid = '%1')").arg(userId);

    query.exec(sql);
    QString uid , uName;
    int i=0;
    while(query.next()){
        uid = query.value(0).toString();
        uName = query.value(1).toString();
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(tr("好友")));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QIcon(":/images/me"), uid));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(uName));
        i++;
    }
    sql = QString("select groupsid, groupsname from groups where groupsid in(select group_id from ingroup where g_uid = '%1')").arg(userId);
    query.exec(sql);
    QString gid , gName;
    while(query.next()){
        gid = query.value(0).toString();
        gName = query.value(1).toString();
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(tr("群聊")));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QIcon(":/images/me"), gid));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(gName));
        i++;
    }
    query.finish();
    query.clear();
    getDB().close();
}
void MainWindow::updateInf(){
    updateInfo *up = new updateInfo(this);
    up->show();
    on_refresh_clicked();
}

void MainWindow::createGp(){
    createGroup *cp = new createGroup(this);
    cp->show();
    on_refresh_clicked();
}

// 实现退出重新登录
void MainWindow::on_quit_clicked()
{
    QSettings settings("WWB-Qt","QChat"); //注册表键组
    settings.setValue("autod", 0); // 关闭自动登陆
    qApp->exit(2);
}

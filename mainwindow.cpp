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

QSqlDatabase getDB(){
    QSqlDatabase dataBase;
    dataBase=QSqlDatabase::addDatabase("QMYSQL3");
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
    this->setFixedSize(400,508);
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
//    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");
    ui->tableWidget->setShowGrid(false); //不显示格子线
    ui->tableWidget->verticalHeader()->setVisible(false);//去掉默认行号
    ui->tableWidget->setStyleSheet("QTableWidget::item {height: 36px;}");
    // 设置选中行背景色
    ui->tableWidget->setStyleSheet("QTableWidget::item:selected {background-color: #F8F0DD;color: #3a3a3a;}");
    //设置无边框
    ui->tableWidget->setFrameShape(QFrame::NoFrame);
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu); //开启右键菜单
    ui->tableWidget->setIconSize(QSize(30,30));
    ui->tableWidget->resizeColumnToContents (0); //根据内容自动调整列宽行高
    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customMenupPop(QPoint)));
    connect(ui->tableWidget, SIGNAL(doubleClicked(QModelIndex)),SLOT(on_openDialogWindow_clicked()));
    connect(ui->exit,SIGNAL(triggered(bool)),this,SLOT(close()));
    connect(ui->updateInfo,SIGNAL(triggered(bool)),this,SLOT(updateInf()));
    connect(ui->createGroup,SIGNAL(triggered(bool)),this,SLOT(createGp()));
    connect(ui->reSign,SIGNAL(triggered(bool)),this,SLOT(on_quit_clicked()));

    // 放到构造函数中，防止每次和一个新的好友聊天都创建一个新的对象
    chatStack = new chatDlgStack(this);
    connect(chatStack, SIGNAL(refreshList()),this,SLOT(on_refresh_clicked()));
    // 发送新用户登录信号
    udpSocket = new QUdpSocket(this);
    port = 45456;
    udpSocket->bind(port,QUdpSocket::ShareAddress
                    | QUdpSocket::ReuseAddressHint);

//    dlgCount =0;
    userImgId = new QStringList;
}

MainWindow::~MainWindow()
{
    delete ui;
}
// 广播上线信息
void MainWindow::sendNewParticipant(){
    QByteArray data;
//    QDataStream out(&data,QIODevice::WriteOnly);
    QString type ="1";
    QString localHostName = "QHostInfo::localHostName()";
    QString UserName = "getIP()";
    data.append(type).append(localHostName).append(UserName);
//    out << type << UserName << localHostName;
    qDebug()<<"\nmainwindow userId"<< userIp << userId<<endl;
//    out << userIp << userId;
//    QString dat; dat.prepend(data);
    qDebug()<<"sendNewParticipant data: "<<data<<data.length()<<endl;
    qint64 res =  udpSocket->writeDatagram(data,data.length(),QHostAddress::Broadcast, port);
    if(res == -1){
        QMessageBox::warning(this,QString("警告"),QString("广播消息失败！"),QMessageBox::Ok);
    }
}

void MainWindow::customMenupPop(QPoint pos){
    QMenu* menu = new QMenu(this);
    QAction *action = new QAction(tr("&Open Directory"), this);
    menu->addAction(action);
    menu->popup(ui->tableWidget->viewport()->mapToGlobal(pos));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(updateGpInf));
}

void MainWindow::updateGpInf(){
    qDebug()<<"updateGpInf"<<endl;
}
// 查找好友群聊
void MainWindow::on_addFriendGroup_clicked()
{
    findGroup *fg = new findGroup(this);
    connect(fg, SIGNAL(refresh()), this,SLOT(on_refresh_clicked()));
    fg->setUId(userId, userName);
    fg->show();
    on_refresh_clicked();
}
// 打开聊天窗口
void MainWindow::on_openDialogWindow_clicked()
{
//    if(dlgCount == 0){

//        dlgCount++;
//    }
    qDebug()<<"on_openDialogWindow_clicked"<<endl;
    if(ui->tableWidget->selectedItems().isEmpty()){
        QMessageBox::warning(0,QString("警告"),QString("你没有选择群聊或好友！"),QMessageBox::Ok);
        return;
    }else{
        QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
        int row=ui->tableWidget->row(items.at(1));//获取选中的行
        QString typeName = items.at(0)->text(), type;
        QString uid = items.at(1)->text();
        QString name = items.at(2)->text();
        QString img = userImgId->at(row);
        QString msg = items.at(3)->text();
        if( msg != "0"){
            ui->tableWidget->item(row,3)->setText("0");
        }
        chatStack->setMeInf(userId, userName);
        if(typeName == "好友"){
            type = "0";
        }else{
            type = "1";
        }
        chatStack->addChatDlg(type, uid, name, img);
        chatStack->show();
    }
    this->on_refresh_clicked();

}
// 刷新好友列表
void MainWindow::on_refresh_clicked()
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->clearContents();
    userImgId->clear();
    QSqlQuery query(getDB());
    if(userId.isEmpty()){
        QMessageBox::warning(0,QString("警告"),QString("账号异常，请重新登录！"),QMessageBox::Ok);
        close();
    }
    QString sql = QString("select uid, userName, headimg from user where uid in"
                          "(select fr_uid from friend where me_uid = '%1')").arg(userId);

    query.exec(sql);
    QString uid , uName, uImg;
    int i=0;
    // 开始插入数据
    while(query.next()){
        uid = query.value(0).toString();
        uName = query.value(1).toString();
        uImg = query.value(2).toString();
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QIcon(QString(":/images/%1").arg(uImg)),tr("好友")));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(uid));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(uName));
        userImgId->append(uImg);
        i++;
    }
    QString senderId;
    // 列表展示未读消息
    for(int i=0; i<userImgId->length(); i++){
        senderId = ui->tableWidget->item(i, 1)->text();
        sql = QString("select count(recvId) from message where senderId = '%1' and  recvId = '%2'").arg(senderId).arg(userId);

//        qDebug()<<"sql: "<<sql<<endl;
        if(!query.exec(sql)){
            QMessageBox::warning(this, tr("警告"),tr("获取数据失败！"),QMessageBox::Ok);
            return;
        }
        int count =0;
        while(query.next()){
            count = query.value(0).toInt();
        }
        // 这里 ui->tableWidget->item(i, 3)->setForeground(Qt::red);
        // ui->tableWidget->item(i, 3)->setText(QString("%1").arg(count));两句为甚么会出错
        //        if(count >0 ){
        //            ui->tableWidget->item(i, 3)->setForeground(Qt::red);
        //        }
        //        ui->tableWidget->item(i, 3)->setText(QString("%1").arg(count));

        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString("%1").arg(count)));
        if(count >0 ){
            ui->tableWidget->item(i, 3)->setForeground(Qt::red);
        }
    }


    sql = QString("select groupsid, groupsname, headimg from groups where groupsid in(select group_id from ingroup where g_uid = '%1')").arg(userId);
    query.exec(sql);
    QString gid , gName, gImg;
    while(query.next()){
        gid = query.value(0).toString();
        gName = query.value(1).toString();
        gImg = query.value(2).toString();
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QIcon(QString(":/images/%1").arg(gImg)), tr("群聊")));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(gid));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(gName));

        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString("0")));
        userImgId->append(gImg);
        i++;
    }
    query.finish();
    query.clear();
    getDB().close();
}
// 修改个人信息
void MainWindow::updateInf(){
    updateInfo *up = new updateInfo(this);
    up->setUserId(userId);
    up->show();
    on_refresh_clicked();
}
// 创建群组
void MainWindow::createGp(){
    createGroup *cp = new createGroup(this);
    cp->setUserId(userId);
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
// 重写关闭事件，增加退出程序时更新用户在线信息
void MainWindow::closeEvent(QCloseEvent *){
    QSqlQuery query(getDB());
    QString sql = QString("update user set status = '%1' where uid = '%2'").arg(0).arg(userId);
    query.exec(sql);
    query.finish();
    query.clear();
    getDB().close();
    QString type = "2";
    QString localHostName = "closeEvent";
    QString UserName = "closeEvent";
    QByteArray data;
    data.append(type).append(localHostName).append(UserName);

    qint64 res =  udpSocket->writeDatagram(data,data.length(),QHostAddress::Broadcast, port);
    if(res == -1){
        QMessageBox::warning(this,QString("警告"),QString("广播消息失败！"),QMessageBox::Ok);
    }
    this->close();
}

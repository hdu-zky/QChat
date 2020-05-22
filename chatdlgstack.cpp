#include "chatdlgstack.h"
#include "ui_chatdlgstack.h"
#include "chatdlg.h"

chatDlgStack::chatDlgStack(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chatDlgStack)
{
    ui->setupUi(this);
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),ui->stackedWidget,SLOT(setCurrentIndex(int)));
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(updateCurrentIndex(int)));
    ui->listWidget->setCurrentRow(0);
    userIdList = new QStringList;
    userNameList = new QStringList;
}

chatDlgStack::~chatDlgStack()
{
    delete ui;
}
void chatDlgStack::updateCurrentIndex(int index){
    // 更新当前活动页窗口序号
    CurrentIndex = index;
}
// 添加新窗口
void chatDlgStack::addChatDlg(QString userId, QString userName){
    // 查找是否已经打开和userName对应的聊天窗口
    bool flag = false;
    for (int i=0; i<userNameList->length(); i++) {
        if(userName == userNameList->at(i)){
            ui->listWidget->setCurrentRow(i);
            setCurrentIndex(i);
            flag=true;break;
        }
    }
    if(!flag){
        // 没有则创建新的
        insertList(userName);
        userNameList->append(userName);
        userIdList->append(userId);
        insertDlg(userId, userName);
        setCurrentIndex(ui->stackedWidget->currentIndex());
    }
}
// listwidget添加项
void chatDlgStack::insertList(QString userName){
    ui->listWidget->addItem(userName);
    ui->listWidget->setCurrentRow(ui->listWidget->currentRow()+1);
}
// stackwidget插入窗口
void chatDlgStack::insertDlg(QString userId, QString userName){
    chatDlg *chat = new chatDlg(ui->stackedWidget);
    // 绑定聊天子窗口的关闭窗口信号，进行子窗口关闭
    connect(chat,SIGNAL(closeSignal()),this,SLOT(closeCurChatDlg()));
    chat->setUserInfo(userId, userName);
    ui->stackedWidget->addWidget(chat);
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex()+1);
    ui->stackedWidget->activateWindow();
}
// 关闭子窗口
void chatDlgStack::closeCurChatDlg(){
    ui->stackedWidget
}

#include "chatdlgstack.h"
#include "ui_chatdlgstack.h"
#include "chatdlg.h"

chatDlgStack::chatDlgStack(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chatDlgStack)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint & ~Qt::WindowContextHelpButtonHint);
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),ui->stackedWidget,SLOT(setCurrentIndex(int)));
    ui->listWidget->setCurrentRow(0);
    // 如果所有图标大小大于28x28则设置起效
    // 设置所有图标大小
    ui->listWidget->setIconSize(QSize(32,32));
    ui->listWidget->setStyleSheet("QListWidget{background: #FFFFF6;border: 1px solid;border-radius:10px;border-color: #F8F0DD;}"
                                  "QListWidget::item{ background: #F5FFFA;border:1px solid;border-radius:6px;border-color:#FFFAF0;}"
                                  "QListWidget::item:selected { background-color:transparent;color :blue;}");

}

chatDlgStack::~chatDlgStack()
{
    delete ui;
}

// 添加新窗口
void chatDlgStack::addChatDlg(QString type, QString userId, QString userName, QString userImg){
    // 查找是否已经打开和userName对应的聊天窗口
    bool flag = false;
    for (int i=0; i<ui->listWidget->count(); i++) {
        if(userName == ui->listWidget->item(i)->text()){
            ui->listWidget->setCurrentRow(i);
            flag=true;break;
        }
    }
    if(!flag){
        // 没有则创建新的
        insertList(userImg, userName);
        insertDlg(type, userId, userName, userImg);
    }
}
// listwidget添加项
void chatDlgStack::insertList(QString userImg, QString userName){
    QListWidgetItem *item = new QListWidgetItem(QIcon(QString(":/images/%1").arg(userImg)),userName);
    item->setSizeHint(QSize(110, 36));  //每次改变Item的高度
//    item->setForeground(Qt::darkBlue);
    item->setFont(QFont("微软雅黑",11));
    item->setBackgroundColor(QColor(245,255,250));
    ui->listWidget->addItem(item);
    ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
}
// stackwidget插入窗口
void chatDlgStack::insertDlg(QString type, QString userId, QString userName, QString userImg){
    chatDlg *chat = new chatDlg(ui->stackedWidget);

    // 绑定聊天子窗口的关闭窗口信号，进行子窗口关闭
    connect(chat,SIGNAL(closeSignal()),this,SLOT(closeCurChatDlg()));
    // 绑定聊天窗口的用户登陆下线信号
//    connect(this, SIGNAL(newParticipantSignal(QString, QString)), chat, SLOT(newParticipant(QString, QString)));
//    connect(this, SIGNAL(participantLeftSignal(QString)), chat, SLOT(participantLeft(QString)));
    qDebug()<<"chatDlgStack::insertDlg meId"<<meId<<endl;
    qDebug()<<"insertDlg type"<<type<<endl;
    chat->setChatType(type);
    chat->setUserInfo(meId, meName, userId, userName, userImg);
    chat->unreadMsg();
    ui->stackedWidget->addWidget(chat);
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->count()-1);
}
// 关闭子窗口
void chatDlgStack::closeCurChatDlg(){
    // 左侧列表移除子窗口的名字列，右侧栈窗口移除子窗口页面

    ui->stackedWidget->removeWidget(ui->stackedWidget->widget(ui->stackedWidget->currentIndex()));
    // 必须先删除右侧子窗口再删除列表数据，因为列表数据变动与右侧窗口展示相关
    delete ui->listWidget->item(ui->listWidget->currentRow());
    if(ui->listWidget->count() == 0 && ui->stackedWidget->count() ==0){
        this->close();
    }else{
        // 如果不添加这一行在删除完列表项和子窗口后会出错
        ui->stackedWidget->setCurrentIndex(ui->stackedWidget->count()-1);
        ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
    }
}
void chatDlgStack::closeEvent(QCloseEvent *){
    emit refreshList();
    this->close();
}

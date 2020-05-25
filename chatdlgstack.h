#ifndef CHATDLGSTACK_H
#define CHATDLGSTACK_H

#include <QDialog>

namespace Ui {
class chatDlgStack;
}

class chatDlgStack : public QDialog
{
    Q_OBJECT

public:
    explicit chatDlgStack(QWidget *parent = 0);
    ~chatDlgStack();

    void addChatDlg(QString type, QString userId, QString userName, QString userImg); // 供外部调用创建新窗口
    void setMeInf(QString mid, QString uname){
        meId = mid;
        meName = uname;
    }

    // 供主窗口实现以下功能
    // 用户登陆后进入主窗口界面,主窗口界面调用newParticipant和participantLeft
    // 两个接口向chatDlgStack栈窗口发送当前登录退出用户信息
    // 栈窗口再把信息传递到相应子窗口UDP数据收发窗口chatDlg
//    void newParticipant(QString ipAddress, QString  recvUserId);
//    void participantLeft(QString recvUserId);

    void insertList(QString userImg,QString userName); // listwidget添加项
    void insertDlg(QString type, QString userId, QString userName, QString userImg); // stackwidget插入窗口
signals:
//    void newParticipantSignal(QString ipAddress, QString  recvUserId);
//    void participantLeftSignal(QString recvUserId);
private slots:
    void updateCurrentIndex(int index);
    void closeCurChatDlg();
private:
    Ui::chatDlgStack *ui;
    QStringList *userIdList, *userNameList;
    QString meId, meName; // 记录当前帐号的id
    int currentIndex;
};

#endif // CHATDLGSTACK_H

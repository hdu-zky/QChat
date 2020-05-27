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
    void insertList(QString userImg,QString userName); // listwidget添加项
    void insertDlg(QString type, QString userId, QString userName, QString userImg); // stackwidget插入窗口
protected:
    void closeEvent(QCloseEvent *);
signals:
    // 主界面刷新列表信号
    void refreshList();
private slots:
    // 关闭当前聊天窗口
    void closeCurChatDlg();
private:
    Ui::chatDlgStack *ui;
    QString meId, meName; // 记录当前帐号的id
};

#endif // CHATDLGSTACK_H

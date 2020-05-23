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

    void addChatDlg(QString userId, QString userName, QString userImg); // 供外部调用创建新窗口
    void insertList(QString userImg,QString userName); // listwidget添加项
    void insertDlg(QString userId, QString userName, QString userImg); // stackwidget插入窗口
private slots:
    void updateCurrentIndex(int index);
    void closeCurChatDlg();
private:
    Ui::chatDlgStack *ui;
    QStringList *userIdList, *userNameList;
    int currentIndex;
};

#endif // CHATDLGSTACK_H

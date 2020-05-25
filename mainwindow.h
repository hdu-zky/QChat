#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "openDB.h"
#include "chatdlgstack.h"

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QStandardItemModel>
#include<QPoint>
#include<QUdpSocket>

class chat;
class QListWidget;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setUserInfo(QString uid, QString uname){
        userId = uid;
        userName = uname;
    }
    void setUserIp(QString ip){
        userIp = ip;
    }

    QString getUserId(){
        return userId;
    }
    void sendNewParticipant();
    void refresh(){
        on_refresh_clicked();
    }
    void updateMsg();
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_addFriendGroup_clicked();
    void on_openDialogWindow_clicked();
    void on_refresh_clicked();
    void updateInf();
    void createGp();
    void on_quit_clicked();
    void customMenupPop(QPoint);
    void updateGpInf();

private:
    Ui::MainWindow *ui;
    QUdpSocket *udpSocket;
    qint16 port; // udpSocket连接的端口号
    chat *dw;
    QString userId, userIp, userName;// 存储当前账号id,ip
    QStringList *userImgId;// 存储好友头像id
    chatDlgStack *chatStack;
    int rawCount, dlgCount;
};

#endif // MAINWINDOW_H

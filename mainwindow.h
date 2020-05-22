#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "user.h"
#include "chatdlgstack.h"

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QStandardItemModel>
#include<QPoint>

class chat;
class QListWidget;
class List;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setUserId(QString uid){
        userId = uid;
        UId = uid;
    }
    QString getUserId(){
        return userId;
    }
    void refresh(){
        on_refresh_clicked();
    }
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
    chat *dw;
    List *list;
    QString userId;
    chatDlgStack *chatStack;
    int rawCount, dlgCount;
};

#endif // MAINWINDOW_H

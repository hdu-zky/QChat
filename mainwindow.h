#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QStandardItemModel>

class DialogWindow;
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
    }
    QString getUserId(){
        return userId;
    }
    void refresh(){
        on_refresh_clicked();
    }
private slots:
    void on_pushButton_clicked();
    void on_addFriendGroup_clicked();
    void on_openDialogWindow_clicked();
    void on_refresh_clicked();

    void on_quit_clicked();

private:
    Ui::MainWindow *ui;
    DialogWindow *dw;
    List *list;
    QString userId;
    int rawCount;
};

#endif // MAINWINDOW_H

#ifndef DIALOGWINDOW_H
#define DIALOGWINDOW_H

#include <QDialog>
#include <QStandardItemModel>

class QListWidget;
namespace Ui {
class DialogWindow;
}

class DialogWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DialogWindow(QWidget *parent = 0);
    ~DialogWindow();
private slots:
    void enableSendBtn();
    void on_send_clicked();

    void on_close_clicked();

private:
    Ui::DialogWindow *ui;
    QStandardItemModel *session;
    int rawCount;
};

#endif // DIALOGWINDOW_H

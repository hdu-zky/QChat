#ifndef RESETPWD_H
#define RESETPWD_H

#include"openDB.h"
#include <QDialog>

namespace Ui {
class resetPwd;
}

class resetPwd : public QDialog
{
    Q_OBJECT

public:
    explicit resetPwd(QWidget *parent = 0);
    ~resetPwd();

private slots:
    void on_btn_Quit_clicked();

    void on_btn_update_clicked();

private:
    Ui::resetPwd *ui;
};

#endif // RESETPWD_H

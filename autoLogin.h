#ifndef AUTOLOGIN_H
#define AUTOLOGIN_H

#include <QDialog>

namespace Ui {
class autoLogin;
}

class autoLogin : public QDialog
{
    Q_OBJECT

public:
    explicit autoLogin(QWidget *parent = 0);
    ~autoLogin();

private slots:
    void on_cancel_clicked();

private:
    Ui::autoLogin *ui;
};

#endif // AUTOLOGIN_H

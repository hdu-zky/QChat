#ifndef UPDATEINFO_H
#define UPDATEINFO_H

#include <QDialog>

namespace Ui {
class updateInfo;
}

class updateInfo : public QDialog
{
    Q_OBJECT

public:
    explicit updateInfo(QWidget *parent = 0);
    ~updateInfo();
    void Init();

private slots:
    void on_btn_Quit_clicked();

    void on_btn_update_clicked();

private:
    Ui::updateInfo *ui;
    QString uId, userName, email, tel;
};

#endif // UPDATEINFO_H

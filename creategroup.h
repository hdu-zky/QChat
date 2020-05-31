#ifndef CREATEGROUP_H
#define CREATEGROUP_H

#include"openDB.h"
#include <QDialog>

namespace Ui {
class createGroup;
}

class createGroup : public QDialog
{
    Q_OBJECT

public:
    explicit createGroup(QWidget *parent = 0);
    ~createGroup();
    void setUserId(QString uId, QString uname){
        userId = uId;
        userName = uname;
    }

private slots:
    void on_btn_Quit_clicked();

    void on_btn_create_clicked();

    void on_btn_chooseImg_clicked();
    void selectImgId(int id);
signals:
    void refresh();
private:
    Ui::createGroup *ui;
    QString userId, userName;
    int imgId;
    QString img;
};

#endif // CREATEGROUP_H

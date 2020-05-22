#ifndef CREATEGROUP_H
#define CREATEGROUP_H

#include"user.h"
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

private slots:
    void on_btn_Quit_clicked();

    void on_btn_create_clicked();

    void on_btn_chooseImg_clicked();
    void selectImgId(int id);

private:
    Ui::createGroup *ui;
    int imgId;
    QString img;
};

#endif // CREATEGROUP_H

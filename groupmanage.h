#ifndef GROUPMANAGE_H
#define GROUPMANAGE_H

#include <QDialog>

namespace Ui {
class groupManage;
}

class groupManage : public QDialog
{
    Q_OBJECT

public:
    explicit groupManage(QWidget *parent = 0);
    ~groupManage();
    void setId(QString mid, QString userId){
        meId = mid;
        groupId = userId;
    }
    void initInfo();
signals:
    void quitGroup();
private slots:
    void on_btn_Quit_clicked();

    void on_btn_delete_clicked();

private:
    Ui::groupManage *ui;
    QString meId, groupId;
};

#endif // GROUPMANAGE_H

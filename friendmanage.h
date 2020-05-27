#ifndef FRIENDMANAGE_H
#define FRIENDMANAGE_H

#include <QDialog>

namespace Ui {
class friendManage;
}

class friendManage : public QDialog
{
    Q_OBJECT

public:
    explicit friendManage(QWidget *parent = 0);
    ~friendManage();
    void setMeId(QString mid, QString fid){
        meId = mid;
        friendId = fid;
    }
    void initInfo();
signals:
    void deleteFriend();
private slots:
    void on_btn_delete_clicked();

    void on_btn_Quit_clicked();

private:
    Ui::friendManage *ui;
    QString meId, friendId;
};

#endif // FRIENDMANAGE_H

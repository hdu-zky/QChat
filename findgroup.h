#ifndef FINDGROUP_H
#define FINDGROUP_H

#include <QDialog>

namespace Ui {
class findGroup;
}

class findGroup : public QDialog
{
    Q_OBJECT

public:
    explicit findGroup(QWidget *parent = 0);
    void setUId(QString uid, QString uname){
        userId = uid;
        userName = uname;
    }
    ~findGroup();
protected:
    void closeEvent(QCloseEvent *);
signals:
    void refresh();
private slots:
    void on_close_clicked();
    void on_add_clicked();
    void enableBtn();
    void enableAddBtn();
    void on_find_clicked();
    void findUser();
    void findGp();
    void on_comboBox_activated(int index);

private:
    Ui::findGroup *ui;
    QString userId, userName;
    int comboIndex;
};

#endif // FINDGROUP_H

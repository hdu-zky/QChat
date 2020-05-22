#ifndef REGISTER_H
#define REGISTER_H

#include <QDialog>
#include <QSqlDatabase>
#include <QSqlQuery>
namespace Ui {
class Register;
}

class Register : public QDialog
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = 0);
    ~Register();

private slots:

    void on_btn_Quit_clicked();

    void on_btn_Register_clicked();

    void on_btn_chooseImg_clicked();
    void selectImgId(int i);
private:
    Ui::Register *ui;
    QSqlDatabase dataBase;
    int imgId;
};

#endif // REGISTER_H

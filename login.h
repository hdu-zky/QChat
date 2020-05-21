#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = 0);
    ~login();
    bool readIniFile();
    void readSetting();
    void saveSetting();
    void readSettings();  // 读取设置,从注册表
    void writeSettings(); // 写入设置，从注册表
    void autoLg();
    QString getIPV4();
    QString getUserId(){
        return userId;
    }
    QString getUserName(){
        return userName;
    }
private slots:
    void on_Login_Button_clicked();
    void on_Register_Button_clicked();
    void changeSaved(); // 密码被编辑,则取消从注册表获得的密码,改用当前编辑的密码
private:
    Ui::login *ui;
    int tryCount=0; // 试错次数
    bool saved;
    QString userId, userName, passWord;
    QSettings *setting;
    void execQuery();
};

#endif // LOGIN_H

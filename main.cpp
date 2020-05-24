#include "mainwindow.h"
#include "login.h"
#include <QApplication>
#include <QTextCodec>
#include <QProcess>
#include <QDebug>

QString gstrFilePath = "";
void relogin(void);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QTextCodec::setCodecForLocale(codec);
    gstrFilePath = QCoreApplication::applicationFilePath();
    MainWindow w;
    login lg;
//    lg.readSettings();
    if(lg.exec() == QDialog::Accepted){
        w.setWindowTitle(QString("欢迎你！%1(%2)").arg(lg.getUserName()).arg(lg.getUserId()));
        w.setUserInfo(lg.getUserId(), lg.getUserName());
        w.setUserIp(lg.getIPV4());
        w.refresh();
        w.sendNewParticipant();
        w.show();
        int nret = a.exec();
        if (nret == 2){
            atexit(relogin);
        }
        return nret;
//        return a.exec();
    }else {
        return 0;
    }
}

//这里启用又一次启动一个新的演示样例
void relogin(){
    QProcess process;
    process.startDetached(gstrFilePath);
}

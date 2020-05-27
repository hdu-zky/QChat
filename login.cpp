#include "login.h"
#include "ui_login.h"
#include "hosturl.h"
#include "openDB.h"
#include "register.h"
#include "autoLogin.h"
#include "resetpwd.h"

#include "QString"       // Qt里的字符串类型
#include "QMessageBox"   // 引入QMessageBox类，用于弹窗
#include <QDialog>
#include <QFile>
#include <QDebug>
#include <QCryptographicHash> // MD5加密库
#include <QFileInfo>
#include <QTimer>
#include <QHostInfo>
#include <QNetworkInterface>

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    this->setFixedSize(310,270);
// //    QMovie *movie = new QMovie(this);
// //    movie->setFileName(":/images/hh");
// //    ui->lb_gif->setMovie(movie);
// //    movie->start();
// //    setWindowFlags(Qt::SplashScreen);
    readSettings(); //读取存储的用户名和密码
//    readIniFile();
//    readSetting();
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint & ~Qt::WindowContextHelpButtonHint);
    connect(ui->UserName,SIGNAL(textEdited(QString)), ui->Password,SLOT(clear()));
}

login::~login()
{
    delete ui;
}
// // 读入ini文件
//bool login::readIniFile(){

//    QFileInfo ini(QCoreApplication::applicationDirPath()+"/config.ini");
//    if(!ini.isFile()){
//        QMessageBox::warning(this,tr("warning"),tr("file %1 not exist")
//                             .arg(ini.fileName()),QMessageBox::Ok); return false;
//    }
//    setting = new QSettings(QCoreApplication::applicationDirPath()+"/config.ini", QSettings::IniFormat); // ini文件
//    if(setting->status() == 1){
//        QMessageBox::warning(this,tr("warning"),tr("trying to write to a read-only file"),QMessageBox::Ok);return false;
//    }else if(setting->status() == 2){
//        QMessageBox::warning(this,tr("warning"),tr("loading a malformed INI file"),QMessageBox::Ok);return false;
//    }
//    return true;
//}
// // 读取设置
//void login::readSetting(){
//    QStringList all = setting->childGroups();
//    if(!all.isEmpty()){
//        // 读取第一组数据
//        setting->beginGroup(all[0]);
//        int save = setting->value("save").toInt();
//        int autoLogin = setting->value("autoLogin").toInt();
//        setting->endGroup();
//        // 更新选项
//        ui->chkBox_Auto->setCheckState(Qt::CheckState(autoLogin));
//        ui->chkBox_Save->setCheckState(Qt::CheckState(save));
//    }
//}
// // 保存设置
//void login::saveSetting()
//{
//    setting->clear();
//    setting->beginGroup("Login");
//    setting->setValue("save", ui->chkBox_Save->checkState());
//    setting->setValue("autoLogin", ui->chkBox_Auto->checkState());
//    setting->endGroup();
//}
// 读取存储的用户名和密码, 密码是经过加密的
void login::readSettings(){
    QString organization="WWB-Qt"; //用于注册表，
    QString appName="QChat"; //HKEY_CURRENT_USER/Software/WWB-Qt/QChat
    QSettings settings(organization, appName); //创建
    QString uName=settings.value("userId","11111111").toString(); // 读取userId键的值，缺省为“user”
    QString pWord=settings.value("passWord","11111111").toString(); // 读取passWord键的值
    saved = settings.value("saved").toBool(); // 读取saved键的值
    bool autod = settings.value("autod",false).toBool(); // 读取auto键的值
    if(!uName.isEmpty()){
        userId = uName;
        ui->UserName->setText(uName);
    }
    if(saved){ // 自动保存密码
        if(!pWord.isEmpty()){
            passWord = pWord;
            ui->Password->setText(pWord);
        }
        ui->chkBox_Save->setChecked(true);
        if(autod){ // 自动登录
            ui->chkBox_Auto->setChecked(true);
            autoLg();
        }
    }else{ // 如果不自动保存密码,则不能自动登录
        ui->chkBox_Save->setChecked(false);
        ui->chkBox_Auto->setChecked(false);
    }
    connect(ui->Password,SIGNAL(textEdited(QString)),this,SLOT(changeSaved()));
}
void login::changeSaved(){
    saved = false;
}
void login::autoLg(){
//    QMessageBox::information(this,tr("提示"),tr("正在自动登陆中......"), QMessageBox::Cancel);

    autoLogin *lt = new autoLogin(this);
    lt->show();
    QTimer::singleShot(2500, lt, SLOT(close()));  // 这里是一个3秒定时器， 且只执行一次。
    execQuery();
//    lt->close();
//    accept();
//    qDebug()<<"autologin"<<endl;
}

// 保存用户名，密码等设置
void login::writeSettings()
{
    QSettings settings("WWB-Qt","QChat"); //注册表键组
    settings.setValue("userId", userId); //用户名
    settings.setValue("saved", ui->chkBox_Save->isChecked());
    settings.setValue("autod", ui->chkBox_Auto->isChecked());
    if(ui->chkBox_Save->isChecked()){
        settings.setValue("passWord", passWord); //密码，经过加密的
    }else{
        settings.setValue("passWord", "");
    }
}
void login::on_Login_Button_clicked()
{
//    accept();
    userId = ui->UserName->text();
    QString pWord = ui->Password->text();
    QString MD5;
    QByteArray pWd;
    pWd = QCryptographicHash::hash(userId.toLatin1(),QCryptographicHash::Md5);
    pWord.append(pWd.toHex());
    pWd = QCryptographicHash::hash(pWord.toLatin1(), QCryptographicHash::Md5);
    MD5.append(pWd.toHex());
//    qDebug()<<userId <<", "<< MD5<<endl;
    if(userId.length()==0 || pWord.length()==0){
        QMessageBox::warning(this, tr("error"), tr("input can't be blank"), QMessageBox::Ok);
        return;
    }
    if(!saved){
        passWord = MD5;
    }
    execQuery();
}
void login::execQuery(){
    QSqlQuery query(getDB());
    QString sql = QString("select uid, userName from user where uid = '%1' and passWord = '%2'").arg(userId).arg(passWord);
    query.exec(sql);
    if(!query.seek(0)){
        QMessageBox::warning(this,tr("警告"),tr("登陆失败！"), QMessageBox::Ok);
        query.finish();
        query.clear();
        getDB().close();
        return;
    }else{
        userId = query.value(0).toString(); // 保存用户名字
        userName = query.value(1).toString(); // 保存用户名字
        writeSettings();
        //更新用户登录IP和在线状态
        sql = QString("update user set ipAddress = '%1', status = '1' where uid = '%2'").arg(getIPV4()).arg(userId);
        if(!query.exec(sql)){
            if(!QSqlDatabase::database().commit()){
                QSqlDatabase::database().rollback(); // 回滚
            }
            QMessageBox::warning(this,tr("警告"), tr("更新用户登录IP和在线状态失败！"),QMessageBox::Ok);
            return;
        }
        query.finish();
        query.clear();
        getDB().close();
        QMessageBox::information(this,tr("提示"),tr("登陆成功！"), QMessageBox::Ok);
//        saveSetting();
        accept();
    }
    return;
}
QString login::getIPV4(){
    QString strIpAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // 获取第一个本主机的IPv4地址
    int nListSize = ipAddressesList.size();
    for (int i = 0; i < nListSize; ++i)
    {
           if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
               ipAddressesList.at(i).toIPv4Address()) {
               strIpAddress = ipAddressesList.at(i).toString();
               break;
           }
     }
//    qDebug()<<strIpAddress<<endl;
    return strIpAddress;
}

void login::on_Register_Button_clicked()
{
    Register reg;
    login lg;

    // to do: hide the windows
    if(reg.exec() == QDialog::Accepted)
    {
        lg.show();//to do: 返回登陆界面或者直接进入主窗口并显示用户信息
    }
}

void login::on_resetPwd_clicked()
{
    resetPwd *rp = new resetPwd(this);
    if(rp->exec() == QDialog::Accepted)
    {
        return;//to do: 返回登陆界面或者直接进入主窗口并显示用户信息
    }
}

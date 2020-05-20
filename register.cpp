#include "register.h"
#include "ui_register.h"
#include <QString>
#include <QMessageBox>
#include <QDateTime>
#include <QCryptographicHash> // MD5加密库
#include <QDebug>

Register::Register(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
//    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
}

Register::~Register()
{
    delete ui;
}
void Register::on_btn_Quit_clicked()
{
    this->close();
}

void Register::on_btn_Register_clicked()
{
    QString uid = ui->input_uid->text();
    QString userName = ui->input_userName->text();
    QString passWord = ui->input_passWord->text();
    QString repwd = ui->input_repwd->text();
    QString email = ui->input_email->text();
    QString tel = ui->input_tel->text();
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString pWord = passWord;
    QString MD5;
    QByteArray pWd;
    pWd = QCryptographicHash::hash(uid.toLatin1(),QCryptographicHash::Md5);
    pWord.append(pWd.toHex());
    pWd = QCryptographicHash::hash(pWord.toLatin1(), QCryptographicHash::Md5);
    MD5.append(pWd.toHex());
    qDebug()<<MD5<<endl;
    if(!userName.length() || !passWord.length() || !repwd.length()){
        QMessageBox::warning(this,tr("warning"), tr("input can't be blank"),QMessageBox::Ok);
        return;
    }
    if(repwd != passWord){
        QMessageBox::warning(this,tr("warning"), tr("the two inputs not the same value"),QMessageBox::Ok);
        return;
    }
    dataBase=QSqlDatabase::addDatabase("QMYSQL3");
    dataBase.setHostName("localhost");
    dataBase.setUserName("root");
    dataBase.setPassword("223412");
    dataBase.setDatabaseName("test");
    dataBase.open();
    QSqlQuery query(dataBase);
    QString sql = QString("select * from user where uid = '%1'").arg(uid);
    query.exec(sql);
    if(query.seek(0)){
        QMessageBox::warning(this,tr("info"),tr("账号已存在！"), QMessageBox::Ok);
        query.finish();
        query.clear();
        dataBase.close();
        return;
    }
    sql = QString("insert into user (uid,userName,passWord,email,tel,signtime) values('%1','%2', '%3','%4','%5','%6')")
            .arg(uid).arg(userName).arg(MD5).arg(email).arg(tel).arg(time);
    // 使数据库支持中文
    query.exec(sql);
    sql = QString("select * from user where uid = '%1'").arg(uid);
    query.exec(sql);
    if(!query.seek(0)){
        QMessageBox::warning(this,tr("info"),tr("Register fail"), QMessageBox::Ok);
        query.finish();
        query.clear();
        dataBase.close();
        return;
    }else{
        query.finish();
        query.clear();
        dataBase.close();
        QMessageBox::warning(this,tr("info"),tr("Register success"), QMessageBox::Ok);
        accept();
    }
    return;
}

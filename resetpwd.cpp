#include "resetpwd.h"
#include "ui_resetpwd.h"

#include<QMessageBox>
#include <QCryptographicHash> // MD5加密库

resetPwd::resetPwd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::resetPwd)
{
    ui->setupUi(this);
    this->resize(300, 300);
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint & ~Qt::WindowContextHelpButtonHint);
}

resetPwd::~resetPwd()
{
    delete ui;
}

void resetPwd::on_btn_Quit_clicked()
{
    this->close();
}

void resetPwd::on_btn_update_clicked()
{
    QString acc, tel, email, pwd, repwd;
    acc = ui->input_acc->text();
    tel = ui->input_tel->text();
    email = ui->input_email->text();
    pwd = ui->input_pwd->text();
    repwd = ui->input_repwd->text();
    if(!acc.length() ||!tel.length() || !email.length() || !pwd.length()|| !repwd.length()){
        QMessageBox::warning(this,tr("警告"), tr("输入不能为空"),QMessageBox::Ok);
        return;
    }
    if(repwd != pwd){
        QMessageBox::warning(this,tr("警告"), tr("两次输入密码不一致！"),QMessageBox::Ok);
        return;
    }
    QString pWord = pwd;
    QString MD5;
    QByteArray pWd;
    pWd = QCryptographicHash::hash(acc.toLatin1(),QCryptographicHash::Md5);
    pWord.append(pWd.toHex());
    pWd = QCryptographicHash::hash(pWord.toLatin1(), QCryptographicHash::Md5);
    MD5.append(pWd.toHex());
    QSqlQuery query(getDB());
    QString sql;
    sql = QString("update user set passWord = '%1' where uid = '%2'").arg(MD5).arg(acc);
    if(query.exec(sql)){
        QMessageBox::information(this,tr("提示"), tr("密码修改成功！"),QMessageBox::Ok);
        this->accept();
    }else{
        if(!QSqlDatabase::database().commit()){
            QSqlDatabase::database().rollback(); // 回滚
        }
        QMessageBox::warning(this,tr("警告"), tr("密码修改失败！"),QMessageBox::Ok);
        return;
    }
}

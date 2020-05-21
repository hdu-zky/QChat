#include "updateinfo.h"
#include "ui_updateinfo.h"
#include "user.h"
#include<QMessageBox>

updateInfo::updateInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::updateInfo)
{
    ui->setupUi(this);
    this->setFixedSize(400,274);
    uId =UId;
    Init();
}

updateInfo::~updateInfo()
{
    delete ui;
}

void updateInfo::on_btn_Quit_clicked()
{
    close();
}
// 获取个人信息并显示在界面上
void updateInfo::Init(){
    QSqlQuery query(getDB());
    QString sql = QString("select userName, email, tel from user where uid = '%1'").arg(uId);
    query.exec(sql);
    if(!query.seek(0)){
        QMessageBox::warning(this,tr("警告"),tr("加载个人信息失败！请重试"), QMessageBox::Ok);
        getDB().close();
        close();
    }else{
        userName = query.value(0).toString(); // 保存用户名字
        email = query.value(1).toString();
        tel = query.value(2).toString();
//        qDebug()<<"login "<< userId<<userName<<endl;
        ui->lb_acc->setText(uId);
        ui->input_userName->setText(userName);
        ui->input_email->setText(email);
        ui->input_tel->setText(tel);
        getDB().close();
    }
}
// 修改信息
void updateInfo::on_btn_update_clicked()
{
    QSqlQuery query(getDB());
    QString sql;
    if(userName != ui->input_userName->text()){
        sql = QString("update user set userName = '%1' where uid = '%2'").arg(ui->input_userName->text()).arg(uId);
        query.exec(sql);
    }
    if(email != ui->input_email->text()){
        sql = QString("update user set email = '%1' where uid = '%2'").arg(ui->input_email->text()).arg(uId);
        query.exec(sql);
    }
    if(tel != ui->input_tel->text()){
        sql = QString("update user set tel = '%1' where uid = '%2'").arg(ui->input_tel->text()).arg(uId);
        query.exec(sql);
    }
    QMessageBox::warning(this,tr("提示"),tr("修改成功！"), QMessageBox::Ok);
}

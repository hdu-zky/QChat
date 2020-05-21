#include "creategroup.h"
#include "ui_creategroup.h"
#include<QDateTime>
#include<QMessageBox>

createGroup::createGroup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::createGroup)
{
    ui->setupUi(this);
    this->setFixedSize(332,249);
}

createGroup::~createGroup()
{
    delete ui;
}

void createGroup::on_btn_Quit_clicked()
{
    close();
}

void createGroup::on_btn_create_clicked()
{
    QString groupsId = ui->input_acc->text();
    QString groupsName = ui->input_name->text();
    QString instruct = ui->input_instruct->text();
    QString img = ui->input_img->text();
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    if(!groupsId.length() || !groupsName.length() || !instruct.length()|| !img.length()){
        QMessageBox::warning(this,tr("warning"), tr("input can't be blank"),QMessageBox::Ok);
        return;
    }
    QSqlQuery query(getDB());
    QString sql, sql0;
    sql = QString("insert into groups (groupsid,creatorid,groupsname,instruct,status,createtime,groupsimgid) values('%1','%2', '%3','%4','%5','%6','%7')")
            .arg(groupsId).arg(UId).arg(groupsName).arg(instruct).arg(1).arg(time).arg(img);
    sql0 = QString("insert into ingroup (g_uid, group_id,join_timing,status,role) values('%1','%2', '%3','%4','%5')")
            .arg(UId).arg(groupsId).arg(time).arg(1).arg(1);
    if(query.exec(sql) && query.exec(sql0)){
        QMessageBox::information(this,tr("提示"),tr("创建群聊成功！"), QMessageBox::Ok);
        accept();
    }else{
        QMessageBox::information(this,tr("提示"),tr("创建群聊失败！"), QMessageBox::Ok);
    }
    query.finish();
    query.clear();
    getDB().close();
}

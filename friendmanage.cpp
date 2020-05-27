#include "friendmanage.h"
#include "ui_friendmanage.h"
#include "openDB.h"
#include<QMessageBox>
#include<QDebug>

friendManage::friendManage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::friendManage)
{
    ui->setupUi(this);
    this->resize(270, 300);
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint & ~Qt::WindowContextHelpButtonHint);
}

friendManage::~friendManage()
{
    delete ui;
}
// 初始化好友信息
void friendManage::initInfo(){
    QSqlQuery query(getDB());
    QString sql;
    qDebug()<<"friendId: "<<friendId<<endl;
    sql = QString("select userName, email, tel, headimg from user where uid= '%1'").arg(friendId);
    qDebug()<<"sql: "<<sql<<endl;
    query.exec(sql);
    if(!query.seek(0)){
        QMessageBox::warning(this,tr("警告"),tr("加载好友信息失败！请重试"), QMessageBox::Ok);
        getDB().close();
        this->close();
    }
    ui->lb_acc->setText(QString("%1").arg(friendId));
    ui->lb_name->setText(query.value(0).toString());
    ui->lb_email->setText(query.value(1).toString());
    ui->lb_tel->setText(query.value(2).toString());
    qDebug()<<"query.value(3).toString()"<<query.value(3).toString()<<endl;
    ui->lb_img->setStyleSheet(QString("border-image: url(':/images/%1')").arg(query.value(3).toString()));


}
// 删除好友
void friendManage::on_btn_delete_clicked()
{
    QSqlQuery query(getDB());
    QString sql, sql1;
    sql = QString("delete from friend where me_uid= '%1' and fr_uid = '%2'").arg(meId).arg(friendId);
    sql1 = QString("delete from friend where me_uid= '%1' and fr_uid = '%2'").arg(friendId).arg(meId);
    int res = QMessageBox::warning(this, tr("警告"),tr("你确定删除该好友吗？"),QMessageBox::Ok | QMessageBox::Cancel);
    if(res == QMessageBox::Cancel){
        return;
    }else{
        if(query.exec(sql)){
            if(query.exec(sql1)){
                QMessageBox::information(this,tr("提示"), tr("好友删除成功！"),QMessageBox::Ok);
                emit deleteFriend();
                this->close();
            }else{
                if(!QSqlDatabase::database().commit()){
                    QSqlDatabase::database().rollback(); // 回滚
                }
                QMessageBox::warning(this,tr("警告"), tr("好友删除失败！"),QMessageBox::Ok);
                return;
            }
        }else{
            if(!QSqlDatabase::database().commit()){
                if(!QSqlDatabase::database().commit()){
                    QSqlDatabase::database().rollback(); // 回滚
                }
            }
            QMessageBox::warning(this,tr("警告"), tr("好友删除失败！"),QMessageBox::Ok);
            return;
        }
    }
}
// 退出窗口
void friendManage::on_btn_Quit_clicked()
{
    this->close();
}

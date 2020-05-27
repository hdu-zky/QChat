#include "groupmanage.h"
#include "ui_groupmanage.h"
#include "openDB.h"
#include<QMessageBox>
#include<QDebug>

groupManage::groupManage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::groupManage)
{
    ui->setupUi(this);
    this->setFixedSize(400,464);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //单击选择一行
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection); //设置只能选择一行，不能多行选中
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);   //设置每行内容不可更改
    ui->tableWidget->setAlternatingRowColors(true);                        //设置隔一行变一颜色，即：一灰一白
    ui->tableWidget->setShowGrid(false); //不显示格子线
    ui->tableWidget->verticalHeader()->setVisible(false);//去掉默认行号
    ui->tableWidget->setStyleSheet("QTableWidget::item {height: 36px;}");
    // 设置选中行背景色
    ui->tableWidget->setStyleSheet("QTableWidget::item:selected {background-color: #F8F0DD;color: #3a3a3a;}");
    //设置无边框
    ui->tableWidget->setFrameShape(QFrame::NoFrame);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu); //开启右键菜单
    ui->tableWidget->setIconSize(QSize(30,30));
    ui->tableWidget->resizeColumnToContents (0); //根据内容自动调整列宽行高
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
}

groupManage::~groupManage()
{
    delete ui;
}
void groupManage::initInfo(){
    QSqlQuery query(getDB());
    QString sql;
    sql = QString("select creatorid, groupsname, headimg, instruct from groups where groupsid= '%1'").arg(groupId);
    qDebug()<<"sql: "<<sql<<endl;
    query.exec(sql);
    if(!query.seek(0)){
        QMessageBox::warning(this,tr("警告"),tr("加载群聊信息失败！请重试"), QMessageBox::Ok);
        getDB().close();
        close();
    }
    ui->lb_acc->setText(QString("%1").arg(groupId));
    ui->lb_name->setText(query.value(1).toString());
    ui->lb_creator->setText(query.value(0).toString());
    ui->lb_instruct->setText(query.value(3).toString());
    ui->lb_img->setStyleSheet(QString("border-image: url(':/images/%1')").arg(query.value(2).toString()));
    sql = QString("select g_uid, userName, role from ingroup where group_id= '%1'").arg(groupId);
    qDebug()<<"sql: "<<sql<<endl;
    query.exec(sql);
    int i=0;
    while(query.next()){
        // 展示用户账号、用户名、群角色
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(query.value(2).toString() == "1"?tr("群主"):tr("群成员")));
        i++;
    }
}

void groupManage::on_btn_Quit_clicked()
{
    this->close();
}

void groupManage::on_btn_delete_clicked()
{
    QSqlQuery query(getDB());
    QString sql;
    sql = QString("delete from ingroup where g_uid= '%1' and group_id = '%2'").arg(meId).arg(groupId);
    int res = QMessageBox::warning(this, tr("警告"),tr("你确定退出该群聊吗？"),QMessageBox::Ok | QMessageBox::Cancel);
    if(res == QMessageBox::Cancel){
        return;
    }else{
        if(query.exec(sql)){
            QMessageBox::information(this,tr("提示"), tr("退出群聊成功！"),QMessageBox::Ok);
            emit quitGroup();
            this->close();
        }else{
            if(!QSqlDatabase::database().commit()){
                if(!QSqlDatabase::database().commit()){
                    QSqlDatabase::database().rollback(); // 回滚
                }
            }
            QMessageBox::warning(this,tr("警告"), tr("退出群聊失败！"),QMessageBox::Ok);
            return;
        }
    }
}

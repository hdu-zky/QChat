#include "findgroup.h"
#include "ui_findgroup.h"
#include "openDB.h"
#include <QDateTime>
#include<QMessageBox>
#include<QDebug>

findGroup::findGroup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::findGroup)
{
    ui->setupUi(this);
    this->setFixedSize(400,360);
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
    ui->result->setSelectionBehavior(QAbstractItemView::SelectRows);  //单击选择一行
    ui->result->setSelectionMode(QAbstractItemView::SingleSelection); //设置只能选择一行，不能多行选中
    ui->result->setEditTriggers(QAbstractItemView::NoEditTriggers);   //设置每行内容不可更改
    ui->result->setAlternatingRowColors(true);                        //设置隔一行变一颜色，即：一灰一白
    ui->find->setEnabled(false);ui->add->setEnabled(false);
    connect(ui->input_id, SIGNAL(textChanged(QString)), this, SLOT(enableBtn()));
    connect(ui->result,SIGNAL(clicked(QModelIndex)),this,SLOT(enableAddBtn()));
}

findGroup::~findGroup()
{
    delete ui;
}
void findGroup::enableBtn(){
    ui->find->setEnabled(true);
}
void findGroup::enableAddBtn(){
    ui->add->setEnabled(true);
}

void findGroup::on_close_clicked()
{
    this->close();
}

void findGroup::on_add_clicked()
{
    QSqlQuery query(getDB());
    QString sql;
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    if(ui->result->selectedItems().isEmpty()){
        QMessageBox::warning(0,QString("警告"),QString("你没有选择群聊或好友！"),QMessageBox::Ok);
        return;
    }else{
        QList<QTableWidgetItem*> items = ui->result->selectedItems();
        QString type = items.at(0)->text(), id = items.at(1)->text();

        if(type == "用户"){
            sql = QString("insert into friend (me_uid, fr_uid,status,create_time) values('%1','%2', '%3','%4')")
                    .arg(userId).arg(id).arg(1).arg(time);
            if(query.exec(sql)){
                QMessageBox::warning(0,QString("提示"),QString("添加成功！你们已经是好友"),QMessageBox::Ok);
//                ui->add->setEnabled(false);
                this->close();
            }else{
                if(!QSqlDatabase::database().commit()){
                    QSqlDatabase::database().rollback(); // 回滚
                }
                QMessageBox::warning(this,tr("警告"), tr("添加失败！"),QMessageBox::Ok);
                return;
            }
        }else{
            sql = QString("insert into ingroup (g_uid, group_id,join_timing,status,role,userName) values('%1','%2','%3','%4','%5','%6')")
                    .arg(userId).arg(id).arg(time).arg(1).arg(0).arg(userName);
            if(query.exec(sql)){
                qDebug()<<"on_add_clicked sql: "<<sql<<endl;
                QMessageBox::warning(0,QString("提示"),QString("添加成功！你已在群里"),QMessageBox::Ok);
//                ui->add->setEnabled(false);
                this->close();
            }else{
                if(!QSqlDatabase::database().commit()){
                    QSqlDatabase::database().rollback(); // 回滚
                }
                QMessageBox::warning(this,tr("警告"), tr("添加失败！"),QMessageBox::Ok);
                return;
            }
        }
    }
    ui->add->setEnabled(false);
}

void findGroup::on_find_clicked()
{
    ui->result->setRowCount(0);
    ui->result->clearContents();
    QString ID = ui->input_id->text();
    if(userId.isEmpty()){
        QMessageBox::warning(0,QString("警告"),QString("账号异常，请重新登录！"),QMessageBox::Ok);
        close();
    }
    if(ID.isEmpty()){
        QMessageBox::warning(0,QString("警告"),QString("查找id不能为空！"),QMessageBox::Ok);
        return;
    }
    if(comboIndex==0){
        findUser();
    }else{
        findGp();
    }
    ui->find->setEnabled(false);
    ui->add->setEnabled(false);
}
void findGroup::findUser(){
    QSqlQuery query(getDB());
    QString ID = ui->input_id->text();
    QString sql = QString("select uid, userName from user where uid = '%1'").arg(ID);
    QString uid,uName;
    query.exec(sql);
    int i = 0;
    while(query.next()){
        uid = query.value(0).toString(); // 保存用户名字
        uName = query.value(1).toString(); // 保存用户名字
        qDebug()<< uid<<uName<<endl;
        ui->result->insertRow(i);
        ui->result->setItem(i, 0, new QTableWidgetItem( tr("用户")));
        ui->result->setItem(i, 1, new QTableWidgetItem(uid));
        ui->result->setItem(i, 2, new QTableWidgetItem(uName));
        i++;
    }

    query.finish();
    query.clear();
    getDB().close();
}
void findGroup::findGp(){
    QSqlQuery query(getDB());
    QString ID = ui->input_id->text();
    QString sql = QString("select groupsid, groupsname from groups where groupsid = '%1'").arg(ID);
    qDebug()<< "sql "<<sql<<endl;
    QString gid,gName;
    query.exec(sql);
    int i = 0;
    qDebug()<< "\nsql exec(sql)"<<endl;
    while(query.next()){
        gid = query.value(0).toString(); // 保存用户名字
        gName = query.value(1).toString(); // 保存用户名字
        qDebug()<< gid<<gName<<endl;
        ui->result->insertRow(i);
        ui->result->setItem(i, 0, new QTableWidgetItem( tr("群聊")));
        ui->result->setItem(i, 1, new QTableWidgetItem(gid));
        ui->result->setItem(i, 2, new QTableWidgetItem(gName));
        i++;
    }
    query.finish();
    query.clear();
    getDB().close();
}
void findGroup::on_comboBox_activated(int index)
{
    comboIndex = index;
    qDebug()<<"index = "<<index<<endl;
}
void findGroup::closeEvent(QCloseEvent *){
    emit refresh();
    this->close();
}

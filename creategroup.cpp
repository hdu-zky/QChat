#include "creategroup.h"
#include "ui_creategroup.h"
#include<QDateTime>
#include<QMessageBox>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

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
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    if(!groupsId.length() || !groupsName.length() || !instruct.length()){
        QMessageBox::warning(this,tr("warning"), tr("input can't be blank"),QMessageBox::Ok);
        return;
    }
    QSqlQuery query(getDB());
    QString sql, sql0;
    sql = QString("insert into groups (groupsid,creatorid,groupsname,instruct,status,createtime,headimg) values('%1','%2', '%3','%4','%5','%6','%7')")
            .arg(groupsId).arg(userId).arg(groupsName).arg(instruct).arg(1).arg(time).arg(imgId);
    sql0 = QString("insert into ingroup (g_uid, group_id,join_timing,status,role) values('%1','%2', '%3','%4','%5')")
            .arg(userId).arg(groupsId).arg(time).arg(1).arg(1);
    if(!query.exec(sql)){
        // 插入第一条语句失败
        if(!QSqlDatabase::database().commit()){
            QSqlDatabase::database().rollback(); // 回滚
        }
        QMessageBox::warning(this,tr("警告"), tr("创建群聊失败！"),QMessageBox::Ok);
        return;
    }else{
        // 插入第二条语句成功
        if(query.exec(sql0)){
            QMessageBox::information(this,tr("提示"),tr("创建群聊成功！"), QMessageBox::Ok);
            accept();
        }else{
            if(!QSqlDatabase::database().commit()){
                QSqlDatabase::database().rollback(); // 回滚
            }
            QMessageBox::warning(this,tr("警告"), tr("创建群聊失败！"),QMessageBox::Ok);
            return;
        }
    }
    query.finish();
    query.clear();
    getDB().close();
}

void createGroup::on_btn_chooseImg_clicked()
{
    QDialog *selectImg = new QDialog(this);
    selectImg->setWindowTitle(tr("选择图片"));
    selectImg->resize(380,300);

    QListWidget *list = new QListWidget(selectImg);
    QPushButton *btn = new QPushButton(selectImg);
    btn->setText(tr("确定"));
    btn->setMinimumWidth(100); btn->setMinimumHeight(30);
    QVBoxLayout *vlayout = new  QVBoxLayout(selectImg);
    vlayout->addWidget(list);
    vlayout->addWidget(btn);
    connect(list, SIGNAL(currentRowChanged(int)), this, SLOT(selectImgId(int)));
    connect(btn, SIGNAL(clicked(bool)), selectImg, SLOT(accept()));
    list->setViewMode(QListView::IconMode);   //设置显示图标模式
    list->setIconSize(QSize(80, 80));         //设置图标大小
    list->setGridSize(QSize(100, 100));       //设置item大小

    // 设置QLisView大小改变时，图标的调整模式，默认是固定的，可以改成自动调整
    list->setResizeMode(QListView::Adjust);   //自动适应布局
    // 列表中的图标默认可以拖动，如果想固定不能拖动，使用QListView::Static
    list->setMovement(QListView::Static);
    //新建item
    QListWidgetItem *it;
    for(int i=1; i<10;i++){
        it = new QListWidgetItem(QIcon(QString(":/images/%1").arg(i)), QString("%1").arg(i));
        //添加item
        list->addItem(it);
    }
    selectImg->show();
}
void createGroup::selectImgId(int id){
    imgId = id+1;
    ui->lb_img->setStyleSheet(QString("border-image: url(':/images/%1')").arg(imgId));
}

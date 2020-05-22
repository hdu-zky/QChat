#include "updateinfo.h"
#include "ui_updateinfo.h"
#include "user.h"
#include<QMessageBox>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>

updateInfo::updateInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::updateInfo)
{
    ui->setupUi(this);
    this->setFixedSize(310,290);
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
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
    QString sql = QString("select userName, email, tel, headimg from user where uid = '%1'").arg(uId);
    query.exec(sql);
    if(!query.seek(0)){
        QMessageBox::warning(this,tr("警告"),tr("加载个人信息失败！请重试"), QMessageBox::Ok);
        getDB().close();
        close();
    }else{
        userName = query.value(0).toString(); // 保存用户名字
        email = query.value(1).toString();
        tel = query.value(2).toString();
        img = query.value(3).toString();
//        qDebug()<<"\n img ="<<img<<endl;
//        qDebug()<<"login "<< userId<<userName<<endl;
        ui->lb_acc->setText(uId);
        ui->input_userName->setText(userName);
        ui->input_email->setText(email);
        ui->input_tel->setText(tel);
        ui->lb_img->setStyleSheet(QString("border-image: url(':/images/%1')").arg(img));
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
    if(img != imgId){
        sql = QString("update user set headimg = '%1' where uid = '%2'").arg(imgId).arg(uId);
        query.exec(sql);
    }
    QMessageBox::warning(this,tr("提示"),tr("修改成功！"), QMessageBox::Ok);
}

void updateInfo::on_btn_chooseImg_clicked()
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
void updateInfo::selectImgId(int id){
    imgId = id+1;
    ui->lb_img->setStyleSheet(QString("border-image: url(':/images/%1')").arg(imgId));
}

#include "register.h"
#include "ui_register.h"
#include <QString>
#include <QMessageBox>
#include <QDateTime>
#include <QCryptographicHash> // MD5加密库
#include <QDebug>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

Register::Register(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    this->setFixedSize(300,380);
//    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint & ~Qt::WindowContextHelpButtonHint);
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

void Register::on_btn_chooseImg_clicked()
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
void Register::selectImgId(int id){
    imgId = id+1;
    ui->lb_img->setStyleSheet(QString("border-image: url(':/images/%1')").arg(imgId));
}


#include "chat.h"
#include "ui_chat.h"
#include "user.h"
#include "filetrans.h"

#include <QListWidget>
#include <QDateTime>
#include <QColorDialog>
#include <QTextDocumentFragment>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QSqlDatabase>
#include <QSqlQuery>

chat::chat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chat)
{
    ui->setupUi(this);
    this->setFixedSize(573,545);
    ui->send->setEnabled(false);
    connect(ui->textEdit,SIGNAL(textChanged()),this,SLOT(enableSendBtn()));
    QSqlDatabase dataBase=QSqlDatabase::addDatabase("QMYSQL3");
    dataBase.setHostName("localhost");
    dataBase.setUserName("root");
    dataBase.setPassword("223412");
    dataBase.setDatabaseName("test");
    dataBase.open();
    QSqlQuery query(dataBase);
    QString email, tel, status, ipAddress, sql;
    sql = QString("select email,tel,status, ipAddress  from user where uid = '%1'").arg(userId);
    if(!query.exec(sql)){
        qDebug()<<"query.lastQuery():"<<query.lastQuery()<<endl;
        return;
    }
    email = query.value(0).toString();
    tel = query.value(1).toString();
    status = query.value(2).toString();
    ipAddress = query.value(3).toString();
    qDebug()<<"ipAddress: "<<email<<tel<<status<<ipAddress<<endl;
    connect(ui->font,SIGNAL(currentFontChanged(QFont)),this,SLOT(on_font_currentFontChanged(QFont)));
    connect(ui->fontSize,SIGNAL(currentIndexChanged(QString)),this,SLOT(on_fontSize_currentIndexChanged(QString)));
    rawCount =0;
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
}

chat::~chat()
{
    delete ui;
}

void chat::enableSendBtn(){
    if(!ui->textEdit->toPlainText().isEmpty()){
        ui->send->setEnabled(true);
    }else{
        ui->send->setEnabled(false);
    }
}

void chat::on_send_clicked()
{
    QString word = ui->textEdit->toHtml();
    QString time = QDateTime::currentDateTime().toString(QString("  yyyy-MM-dd hh:mm:ss"));
    QString user = QString(QString("<b style='color: darkBlue' >[%1]</b><span style='color: grey'>(<u>%2</u>)<i>%3</i></span>")
                           .arg(userName).arg(userId).arg(time));
//    QString time = QDateTime::currentDateTime().toString(QString("[%1](%2)").arg(userName).arg(userId)
//                                                         +"  yyyy-MM-dd hh:mm:ss");

//    session->setItem(rawCount++, new QStandardItem(QIcon(":/images/me"), time));
//    session->setItem(rawCount++, new QStandardItem(word));
//    // 根据内容自动调整行高
//    ui->tableView->resizeRowToContents(rawCount-1);

//    ui->textBrowser->setTextColor(Qt::darkBlue);
    ui->textBrowser->append(user);
    ui->textBrowser->append(word);
    ui->textEdit->clear();
    ui->send->setEnabled(false);
}

void chat::on_close_clicked()
{
    this->close();
}
void chat::on_font_currentFontChanged(const QFont &f)
{
    ui->textEdit->setCurrentFont(f);
    ui->textEdit->setFocus();
}

void chat::on_fontSize_currentIndexChanged(const QString &size)
{
    ui->textEdit->setFontPointSize(size.toDouble());
    ui->textEdit->setFocus();
}

void chat::on_textitalic_clicked(bool checked)
{
    ui->textEdit->setFontItalic(checked);
    ui->textEdit->setFocus();
}

void chat::on_textUnderline_clicked(bool checked)
{
    ui->textEdit->setFontUnderline(checked);
    ui->textEdit->setFocus();
}

void chat::on_textcolor_clicked()
{
    color = QColorDialog::getColor(color,this);
    if(color.isValid())
    {
        ui->textEdit->setTextColor(color);
        ui->textEdit->setFocus();
    }
}

void chat::on_textbold_clicked(bool checked)
{
    if(checked)
        ui->textEdit->setFontWeight(QFont::Bold);
    else
        ui->textEdit->setFontWeight(QFont::Normal);
    ui->textEdit->setFocus();
}
// 打开文件传输窗口
void chat::on_sendfile_clicked()
{
    fileTrans *ft = new fileTrans(this);
    ft->show();

}
// 保存聊天记录
void chat::on_save_clicked()
{
    if(ui->textBrowser->document()->isEmpty()){
        QMessageBox::warning(this, tr("警告"), tr("聊天记录为空！"),QMessageBox::Ok);
    }else{
        QString fileName = QFileDialog::getSaveFileName(this, tr("保存为"), QString("%1-%2").arg(UId).arg(userId),//将用户id和好友id作为文件名
                                                        tr("Html文件(*.html);;文本(*.rtf);;所有类型(*.*)"));
        if(!fileName.isEmpty()){
            QFile file(fileName);
            if(!file.open(QFile::WriteOnly | QFile::Text)){
                QMessageBox::warning(this,QString("警告"),
                                     QString("不能保存文件%1:\n %2").arg(fileName)
                                     .arg(file.errorString()));
                return;
            }
            QTextStream out(&file);
            out << ui->textBrowser->toHtml();
        }
    }
}
// 清空聊天记录
void chat::on_clear_clicked()
{
    ui->textBrowser->clear();
}



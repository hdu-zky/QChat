#include "chatdlg.h"
#include "ui_chatdlg.h"
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

chatDlg::chatDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chatDlg)
{
    ui->setupUi(this);
    this->setFixedSize(580,550);
    ui->send->setEnabled(false);
    ui->textEdit->setStyleSheet("background: #FFFFF6;border: 1px solid;border-radius:10px;border-color: #F8F0DD;");
    ui->textBrowser->setStyleSheet("background: #FFFFF6;border: 1px solid;border-radius:10px;border-color: #F8F0DD;");
    connect(ui->textEdit,SIGNAL(textChanged()),this,SLOT(enableSendBtn()));
    connect(ui->font,SIGNAL(currentFontChanged(QFont)),this,SLOT(on_font_currentFontChanged(QFont)));
    connect(ui->fontSize,SIGNAL(currentIndexChanged(QString)),this,SLOT(on_fontSize_currentIndexChanged(QString)));
    rawCount =0;
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);

    udpSocket = new QUdpSocket(this);
    port = 45454;
    udpSocket->bind(port,QUdpSocket::ShareAddress
                    | QUdpSocket::ReuseAddressHint);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));
    sendMessage(NewParticipant);

    server = new fileTrans(this);
    connect(server,SIGNAL(sendFileName(QString)),this,SLOT(sentFileName(QString)));
}

chatDlg::~chatDlg()
{
    delete ui;
}
void chatDlg::processPendingDatagrams()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size());
        QDataStream in(&datagram,QIODevice::ReadOnly);
        int messageType;
        in >> messageType;
        QString userName,localHostName,ipAddress,message;
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        switch(messageType)
        {
//        case Message:
//        {
//            in >>userName >>localHostName >>ipAddress >>message;
//            ui->textBrowser->setTextColor(Qt::blue);
//            ui->textBrowser->setCurrentFont(QFont("Times New Roman",12));
//            ui->textBrowser->append("[ " +userName+" ] "+ time);
//            ui->textBrowser->append(message);
//            break;
//        }
        case FileName:
        {
            in >>userName >>localHostName >> ipAddress;
            QString clientAddress,fileName;
            in >> clientAddress >> fileName;
            hasPendingFile(userName,ipAddress,clientAddress,fileName);
            break;
        }
        case Refuse:
        {
            in >> userName >> localHostName;
            QString serverAddress;
            in >> serverAddress;
            QString ipAddress = getIP();

            if(ipAddress == serverAddress)
            {
                server->refused();
            }
            break;
        }
        }
    }
}
QString chatDlg::getIP(){
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list){
        if(address.protocol() == QAbstractSocket::IPv4Protocol){
            return address.toString();
        }
    }
    return 0;
}
void chatDlg::sendMessage(MessageType type, QString serverAddress){
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString localHostName = QHostInfo::localHostName();
    QString address = getIP();
    out << type << getUserName() << localHostName;

    switch(type){
        case ParticipantLeft:{
                break;
            }
        case NewParticipant:{
                out << address;
                break;
            }
        case Message :{
                if(ui->textEdit->toPlainText() == "")
                {
                    QMessageBox::warning(0,QString("警告"),QString("发送消息为空！"),QMessageBox::Ok);
                    return;
                }
                out << address << getMessage();
//                ui->textBrowser->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->maximum());
                break;

            }
        case FileName:{
                QSqlQuery query(getDB());
                QString sql = QString("select ip from user where uid = '%1'").arg(userId);
                qDebug()<<"userid: "<<userId<<endl;
                query.exec(sql);
                int clientAddress = query.value(0).toInt();
                query.finish();
                query.clear();
                getDB().close();
                out << address << clientAddress << fileName;
                break;
            }
        case Refuse:{
                out << serverAddress;
                break;
            }
    }
    udpSocket->writeDatagram(data,data.length(),QHostAddress::Broadcast, port);

}
QString chatDlg::getUserName()
{
    QStringList envVariables;
    envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
                 << "HOSTNAME.*" << "DOMAINNAME.*";
    QStringList environment = QProcess::systemEnvironment();

    foreach (QString string, envVariables){
        int index = environment.indexOf(QRegExp(string));
        if (index != -1){
            QStringList stringList = environment.at(index).split('=');
            if (stringList.size() == 2){
                return stringList.at(1);
                break;
            }
        }
    }
    return NULL;
}
QString chatDlg::getMessage()
{
    QString msg = ui->textEdit->toHtml();

    ui->textEdit->clear();
    ui->textEdit->setFocus();
    return msg;
}
void chatDlg::sentFileName(QString fileName)
{
    this->fileName = fileName;
    sendMessage(FileName);
}
void chatDlg::hasPendingFile(QString userName,QString serverAddress,
                            QString clientAddress,QString fileName)
{
    QString ipAddress = getIP();

    if(ipAddress == clientAddress){
        int btn = QMessageBox::information(this,QString("提示"),
                                           QString("来自%1(%2)的文件%3,接收吗")
                                           .arg(userName).arg(serverAddress).arg(fileName),
                                           QMessageBox::Yes,QMessageBox::No);
        if(btn == QMessageBox::Yes){
            QString name = QFileDialog::getSaveFileName(0,QString("???????"),fileName);

            if(!name.isEmpty()){
                fileRecv *client = new fileRecv(this);
                client->setFileName(name);
                client->setHostAddress(QHostAddress(serverAddress));
                client->show();
            }
        }else{
            sendMessage(Refuse,serverAddress);
        }
    }
}
void chatDlg::setUserInfo(QString uid, QString uname){
    userId = uid;
    userName = uname;
    ui->checkInfo->setText(QString("%1(%2)").arg(uname).arg(uid));
}
void chatDlg::enableSendBtn(){
    if(!ui->textEdit->toPlainText().isEmpty()){
        ui->send->setEnabled(true);
    }else{
        ui->send->setEnabled(false);
    }
}

void chatDlg::on_send_clicked()
{
    QString word = ui->textEdit->toHtml();
    QString time = QDateTime::currentDateTime().toString(QString("  yyyy-MM-dd hh:mm:ss"));
    QString user = QString(QString("<b style='color: darkBlue' >[%1]</b><span style='color: grey'>(<u>%2</u>)<i>%3</i></span>")
                           .arg(userName).arg(userId).arg(time));
//    ui->textBrowser->setTextColor(Qt::darkBlue);
    ui->textBrowser->append(user);
    ui->textBrowser->append(word);
    ui->textEdit->clear();
    ui->send->setEnabled(false);
}

void chatDlg::on_close_clicked()
{
    // 给栈窗体发送关闭信号
    ui->close->setEnabled(false);
    emit closeSignal();
    this->close();
}
void chatDlg::on_font_currentFontChanged(const QFont &f)
{
    ui->textEdit->setCurrentFont(f);
    ui->textEdit->setFocus();
}

void chatDlg::on_fontSize_currentIndexChanged(const QString &size)
{
    ui->textEdit->setFontPointSize(size.toDouble());
    ui->textEdit->setFocus();
}

void chatDlg::on_textitalic_clicked(bool checked)
{
    ui->textEdit->setFontItalic(checked);
    ui->textEdit->setFocus();
}

void chatDlg::on_textUnderline_clicked(bool checked)
{
    ui->textEdit->setFontUnderline(checked);
    ui->textEdit->setFocus();
}

void chatDlg::on_textcolor_clicked()
{
    color = QColorDialog::getColor(color,this);
    if(color.isValid())
    {
        ui->textEdit->setTextColor(color);
        ui->textEdit->setFocus();
    }
}

void chatDlg::on_textbold_clicked(bool checked)
{
    if(checked)
        ui->textEdit->setFontWeight(QFont::Bold);
    else
        ui->textEdit->setFontWeight(QFont::Normal);
    ui->textEdit->setFocus();
}
// 打开文件传输窗口
void chatDlg::on_sendfile_clicked()
{
    fileTrans *ft = new fileTrans(this);
    ft->show();

}
// 保存聊天记录
void chatDlg::on_save_clicked()
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
void chatDlg::on_clear_clicked()
{
    ui->textBrowser->clear();
}


#include "chatdlg.h"
#include "ui_chatdlg.h"
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
    ui->textEdit->setFocusPolicy(Qt::StrongFocus);
    ui->textBrowser->setFocusPolicy(Qt::NoFocus);
    ui->lb_spoint->setStyleSheet("border: 1px solid;border-radius: 5px;");
    ui->textEdit->setStyleSheet("background: #FFFFF6;border: 1px solid;border-radius:10px;border-color: #F8F0DD;");
    ui->textBrowser->setStyleSheet("background: #FFFFF6;border: 1px solid;border-radius:10px;border-color: #F8F0DD;");
    ui->textEdit->setFocus();
    connect(ui->textEdit,SIGNAL(textChanged()),this,SLOT(enableSendBtn()));
    connect(ui->font,SIGNAL(currentFontChanged(QFont)),this,SLOT(on_font_currentFontChanged(QFont)));
    connect(ui->fontSize,SIGNAL(currentIndexChanged(QString)),this,SLOT(on_fontSize_currentIndexChanged(QString)));
    rawCount =0;
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);

    udpSocket = new QUdpSocket(this);
    port = 45456;
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
// 接收udp消息
void chatDlg::processPendingDatagrams()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        // 接收一个数据报并将其存储在data中，返回的是数据报的长度
        // 将读取到的不大于datagram.size()大小的数据输入到datagram.data()中
        udpSocket->readDatagram(datagram.data(),datagram.size());
        QDataStream in(&datagram,QIODevice::ReadOnly);
        int messageType;
        // 读入消息类型数据
        // in>>后面如果为QString，则表示读取一个知道出现'\0'的字符串
        in >> messageType;

        QString userName,localHostName,ipAddress,message;
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        switch(messageType)
        {
        case Message:
        {
            in >>userName >>localHostName >>ipAddress >>message;
            qDebug()<<"\nchatDlg::processPendingDatagrams message: "<<userName<<localHostName<<ipAddress<<message<<endl;
            ui->textBrowser->setTextColor(Qt::blue);
            ui->textBrowser->setCurrentFont(QFont("微软雅黑",15));
            ui->textBrowser->append("[ " +userName+" ] "+ time);
            ui->textBrowser->setCurrentFont(QFont("微软雅黑",13));
            ui->textBrowser->append(message);
            break;
        }
        case FileName:
        {
            in >>userName >>localHostName >> ipAddress;
            QString clientAddress,fileName;
            in >> clientAddress >> fileName;
            qDebug()<<"\nchatDlg::processPendingDatagrams FileName: "<<"userName"<<userName<<"localHostName"<<localHostName<<endl;
            qDebug()<<"ipAddress: "<<ipAddress<<"clientAddress"<<clientAddress<<"fileName"<<fileName<<endl;
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
// 获得本机IP
QString chatDlg::getIP(){
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list){
        if(address.protocol() == QAbstractSocket::IPv4Protocol){
            return address.toString();
        }
    }
    return 0;
}
// 发送udp消息
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
                qDebug()<<"userId: "<<userId<<endl;
                QString sql, clientAddress, userName;
                sql = QString("select userName, ipAddress  from user where uid = '%1'").arg(userId);
                if(!query.exec(sql)){
                    qDebug()<<"query.lastQuery():"<<query.lastQuery()<<endl;
                    return;
                }

                // 使用数据时一定要query.next()，不然根本读取不到，这个简单的问题困扰了愚蠢的我很久
                while(query.next()){
                    userName = query.value(0).toString();
                    clientAddress = query.value(1).toString();
                }
                qDebug()<<"clientAddress: "<<clientAddress<<"userName: "<<userName<<endl;
                if(clientAddress.isEmpty()){
                    QMessageBox::warning(this, QString("警告"), QString("目标ip地址为空！"),QMessageBox::Ok);
                    return;
                }
                out << address << clientAddress << fileName;
                query.finish();
                query.clear();
                getDB().close();
                break;
            }
        case Refuse:{
                out << serverAddress;
                break;
            }
    }
    // QHostAddress::Broadcast是指发送数据的目的地址
    udpSocket->writeDatagram(data,data.length(),QHostAddress::Broadcast, port); //目标地址，QHostAddress::Broadcast广播发送

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
    qDebug()<<"chatDlg::sentFileName"<<fileName<<endl;
    this->fileName = fileName;
    QString recvAddress = "169.254.166.189";
    sendMessage(FileName, userId);
}
void chatDlg::hasPendingFile(QString userName,QString serverAddress,
                            QString clientAddress,QString fileName)
{
    QString ipAddress = getIP();
    qDebug()<<"chatDlg::hasPendingFile ipAddress: "<<ipAddress<<"clientAddress"<<clientAddress<<endl;
    if(ipAddress == clientAddress){
        int btn = QMessageBox::information(this,QString("提示"),
                                           QString("来自%1(%2)的文件%3,接收吗")
                                           .arg(userName).arg(serverAddress).arg(fileName),
                                           QMessageBox::Yes,QMessageBox::No);
        if(btn == QMessageBox::Yes){
            QString name = QFileDialog::getSaveFileName(0,QString("文件保存为"),fileName);

            if(!name.isEmpty()){
                fileRecv *client = new fileRecv(this);
                client->setFileName(name);
                // serverAddress 发送方IP地址
                client->setHostAddress(QHostAddress(serverAddress));
                client->show();
            }
        }else{
            sendMessage(Refuse,serverAddress);
        }
    }
}
void chatDlg::setUserInfo(QString uid, QString uname, QString imgId){
    userId = uid;
    userName = uname;
    userimgId = imgId;
    QSqlQuery query(getDB());
    QString sql;
    qDebug()<<"userId: "<<userId<<endl;
    sql = QString("select email, tel, status, ipAddress  from user where uid = '%1'").arg(userId);
    if(!query.exec(sql)){
        qDebug()<<"query.lastQuery():"<<query.lastQuery()<<endl;
        return;
    }
    // 使用数据时一定要query.next()，不然根本读取不到，这个简单的问题困扰了愚蠢的我很久
    while(query.next()){
        useremail = query.value(0).toString();
        usertel = query.value(1).toString();
        userstatus = query.value(2).toString();
        useripAddress = query.value(3).toString();
    }
    ui->checkInfo->setText(QString("%1(%2)").arg(uname).arg(uid));
    ui->lb_img->setStyleSheet(QString("border-image: url(':/images/%1')").arg(imgId));
    ui->lb_name->setText(uname);
    if(userstatus == "1"){
        ui->lb_status->setText(QString("在线"));
        ui->lb_spoint->setStyleSheet("background-color: #92bd6c");
        ui->lb_status->setStyleSheet("color: green");
    }else{
        ui->lb_status->setText(QString("离线"));
        ui->lb_spoint->setStyleSheet("background-color: #f87878");
        ui->lb_status->setStyleSheet("color: gray");
    }
    ui->lb_email->setText(QString("邮箱: %1").arg(useremail));
    ui->lb_tel->setText(QString("电话: %1").arg(usertel));
    ui->lb_instruct->setText(useripAddress);
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
    sendMessage(Message);
//    QString word = ui->textEdit->toHtml();
//    QString time = QDateTime::currentDateTime().toString(QString("  yyyy-MM-dd hh:mm:ss"));
//    QString user = QString(QString("<b style='color: darkBlue' >[%1]</b><span style='color: grey'>(<u>%2</u>)<i>%3</i></span>")
//                           .arg(userName).arg(userId).arg(time));
// //    ui->textBrowser->setTextColor(Qt::darkBlue);
//    ui->textBrowser->append(user);
//    ui->textBrowser->append(word);
//    ui->textEdit->clear();
    ui->send->setEnabled(false);
}
// 打开文件传输窗口
void chatDlg::on_sendfile_clicked()
{
//    fileTrans *ft = new fileTrans(this);
//    ft->show();
    server->show();
    server->initServer();

}
void chatDlg::on_close_clicked()
{
    // 给栈窗体发送关闭信号
    ui->close->setEnabled(false);
    emit closeSignal();
    udpSocket->close();
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


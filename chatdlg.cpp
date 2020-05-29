#include "chatdlg.h"
#include "ui_chatdlg.h"
#include "filetrans.h"

#include "friendmanage.h"
#include "groupmanage.h"

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
//    ui->lb_spoint->setStyleSheet("border: 1px solid;border-radius: 4px;");
    ui->lb_name->setStyleSheet("color: blue;");
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
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(pendingDatagrams()));

    server = new fileTrans(this);
    connect(server,SIGNAL(sendFileName(QString)),this,SLOT(sentFileName(QString)));

//    useripAddress = "127.0.0.1";

}

chatDlg::~chatDlg()
{
    delete ui;
}
// 查找展示未读消息
void chatDlg::unreadMsg(){
    if(chatType == "1"){
        return;
    }
    QSqlQuery query(getDB());
    QString sql = QString("select content, sendTime from message where senderId = '%1' and recvId = '%2'").arg(userId).arg(meId);
    query.exec(sql);
    QString content, sendTime;
    if(query.size()){
        ui->textBrowser->setTextColor(Qt::red);
        ui->textBrowser->append(QString("以下为%1条未读消息").arg(query.size()));
    }
    while(query.next()){
        content = query.value(0).toString();
        sendTime = query.value(1).toString();
        ui->textBrowser->setTextColor(Qt::darkGray);
        ui->textBrowser->setCurrentFont(QFont("微软雅黑",13));
        ui->textBrowser->append( "["+userName+"]" + "("+userId+")" + QString("%1").arg(sendTime));
        ui->textBrowser->setTextColor(Qt::darkGreen);
        ui->textBrowser->setCurrentFont(QFont("幼圆",12));
        ui->textBrowser->append(content);
    }
    // 读取完成后删除记录
    sql = QString("delete from message where senderId = '%1' and recvId = '%2'").arg(userId).arg(meId);
    if(!query.exec(sql)){
        if(!QSqlDatabase::database().commit()){
            QSqlDatabase::database().rollback(); // 回滚
        }
        QMessageBox::warning(this,tr("警告"), tr("消息删除失败！"),QMessageBox::Ok);
        return;
    }
    query.finish();
    query.clear();
    getDB().close();
}

// 接收udp消息
void chatDlg::pendingDatagrams()
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
        QString userName,ipAddress,message,sendUserId,recvUserId, chatTypeName;
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        switch(messageType){
        case Message:{
            in >>userName >>ipAddress >>message>>sendUserId>>recvUserId>>chatTypeName;
            qDebug()<<"chatDlg::pendingDatagrams message: "
                   <<userName<<ipAddress<<sendUserId<<recvUserId<<endl;
            qDebug()<<"userId: meId"<<userId<<meId;
            bool flag =false;
            // 如果是发送方id等于当前账号id，则消息已显示，不在重复显示
            if(sendUserId == meId){
                return;
            }
            // 当是好友私聊时
            if(chatTypeName == '0'){
                // 如果发送发id等于当前对话窗口id
                if(sendUserId == userId){
                    ui->textBrowser->setTextColor(Qt::blue);
                    ui->textBrowser->setCurrentFont(QFont("微软雅黑",13));
                    ui->textBrowser->append("[" +userName+"]"+ "("+userId+")" + time);
                    ui->textBrowser->setCurrentFont(QFont("微软雅黑",12));
                    ui->textBrowser->append(message);
                    flag =true;
                }
            }else{
                // 如果是群聊，接收方id等于当前对话窗口id
                if(userId == recvUserId && !flag){
                    ui->textBrowser->setTextColor(Qt::blue);
                    ui->textBrowser->setCurrentFont(QFont("微软雅黑",13));
                    ui->textBrowser->append("[" +userName+"]"+ "("+sendUserId+")" + time);
                    ui->textBrowser->setCurrentFont(QFont("微软雅黑",12));
                    ui->textBrowser->append(message);
                }
            }
            break;
        }
        case NewParticipant:{
            in >>userName >>ipAddress >> recvUserId;
            newParticipant(ipAddress, recvUserId);
            qDebug()<<"NewParticipant:"<<userName <<ipAddress<<recvUserId<<endl;
            break;
        }
        case ParticipantLeft:{
            in >>userName >>ipAddress >> recvUserId;
            participantLeft(recvUserId);
            qDebug()<<"ParticipantLeft:"<<userName<<ipAddress<<recvUserId<<endl;
            break;
        }
        case FileName:{
            in >>userName >> ipAddress;
            QString clientAddress,fileName;
            in >> clientAddress >> fileName;
            qDebug()<<"\nchatDlg::pendingDatagrams FileName: "<<"userName"<<userName<<endl;
            qDebug()<<"ipAddress: "<<ipAddress<<"clientAddress"<<clientAddress<<"fileName"<<fileName<<endl;
            hasPendingFile(userName,ipAddress,clientAddress,fileName);
            break;
        }
        case Refuse:{
            in >> userName;
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
// 当有新用户登陆时，查看是否打开了和他的聊天界面
void chatDlg::newParticipant(QString ipAddress, QString  recvUserId){
    // 如果当前界面的聊天好友是新登陆了
    if(recvUserId == userId){
        // 更新他的IP地址
        useripAddress = ipAddress;
        userstatus = "0";
        // 更新与他的对话框的在线信息
        ui->lb_status->setText(QString("在线"));
        ui->lb_spoint->setStyleSheet("background-color: #92bd6c");
        ui->lb_status->setStyleSheet("color: green");
    }
}
// 当有用户离开时
void chatDlg::participantLeft(QString recvUserId){
    // 如果当前界面的聊天好友离线了
    if(recvUserId == userId){
        // 更新他的在线状态
        userstatus = "0";
        // 更新与他的对话框的在线信息
        ui->lb_status->setText(QString("离线"));
        ui->lb_spoint->setStyleSheet("background-color: #f87878");
        ui->lb_status->setStyleSheet("color: gray");
    }
}

// 发送udp消息
void chatDlg::sendMessage(MessageType type, QString serverAddress){
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString address = getIP();
    qDebug()<<"sendMessage type:"<<type<<endl;
    out << type << meName;
    switch(type){
        case ParticipantLeft:{
            out << address << meId;
            break;
        }
        case NewParticipant:{
            out << address << meId;
            break;
        }
        case Message :{
                // 写入发送目标id in >>userName >>ipAddress >>message>>recvUserId;
                out << address << getMessage()<< meId<<userId<<chatType;
                qDebug()<<"Message useripAddress: "<<address<<useripAddress<<meId<<userId<<endl;
                break;

            }
        case FileName:{
                if(useripAddress.isEmpty()){
                    QMessageBox::warning(this, QString("警告"), QString("目标ip地址为空！"),QMessageBox::Ok);
                    return;
                }
                qDebug()<<"address:useripAddress:fileName "<<address<<useripAddress<<fileName<<endl;
                out << address << useripAddress << fileName<<meId;
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

// 获取输入框的数据
QString chatDlg::getMessage()
{
    return ui->textEdit->toHtml();
}
void chatDlg::sentFileName(QString fileName)
{
    qDebug()<<"chatDlg::sentFileName"<<fileName<<endl;
    this->fileName = fileName;
    sendMessage(FileName);

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
//                Server *sr = new Server(this);
//                sr->show();

            }
        }else{
            sendMessage(Refuse,serverAddress);
        }
    }
}
void chatDlg::setUserInfo(QString mid, QString mname, QString uid, QString uname, QString imgId){
    meId = mid;
    meName = mname;
    userId = uid;
    userName = uname;
    userimgId = imgId;
    if(chatType == "1"){
        ui->sendfile->setEnabled(false);
    }
    QSqlQuery query(getDB());
    QString sql;
    qDebug()<<"meId:"<<meId<<"userId: "<<userId<<endl;
    sql = QString("select email, tel, status, signature, ipAddress  from user where uid = '%1'").arg(userId);
    if(!query.exec(sql)){
        qDebug()<<"query.lastQuery():"<<query.lastQuery()<<endl;
        return;
    }
    // 使用数据时一定要query.next()，不然根本读取不到，这个简单的问题困扰了愚蠢的我很久
    while(query.next()){
        useremail = query.value(0).toString();
        usertel = query.value(1).toString();
        userstatus = query.value(2).toString();
        usersignature = query.value(3).toString();
        useripAddress = query.value(4).toString();
    }
//    ui->checkInfo->setText(QString("%1(%2)").arg(uname).arg(uid));
    ui->lb_img->setStyleSheet(QString("border-image: url(':/images/%1')").arg(imgId));
    ui->lb_name->setText(uname);
    qDebug()<<"chatType"<<chatType<<endl;
    if(chatType != "0"){
        return;
    }
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
    ui->lb_instruct->setText(QString("个性签名: %1").arg(usersignature));
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
void chatDlg::enableSendBtn(){
    if(!ui->textEdit->toPlainText().isEmpty()){
        ui->send->setEnabled(true);
    }else{
        ui->send->setEnabled(false);
    }
}
// 点击发送按钮后
void chatDlg::on_send_clicked()
{
    if(ui->textEdit->toPlainText() == ""){
        QMessageBox::warning(0,QString("警告"),QString("发送消息为空！"),QMessageBox::Ok);
        return;
    }
    QString message = getMessage();
    QString time = QDateTime::currentDateTime().toString(QString("  yyyy-MM-dd hh:mm:ss"));
//    QString user = QString(QString("<b style='color: darkBlue' >[%1]</b><span style='color: grey'>(<u>%2</u>)<i>%3</i></span>")
//                           .arg(userName).arg(userId).arg(time));
//     ui->textBrowser->setTextColor(Qt::darkBlue);
    // 当前对话好友不在线
    if(userstatus == '0'){
        QSqlQuery query(getDB());
        QString sql, msg = ui->textEdit->toPlainText();
        sql = QString("insert into message (senderId, recvId, content, sendTime, state) values('%1', '%2', '%3', '%4', '%5')").arg(meId).arg(userId).arg(msg).arg(time).arg('1');
        if(!(query.exec(sql))){
            if(!QSqlDatabase::database().commit()){
                QSqlDatabase::database().rollback(); // 回滚
            }
            QMessageBox::warning(this, tr("警告"),tr("消息发送失败"),QMessageBox::Ok);
            return;
        }
        query.finish();
        query.clear();
    }
    ui->textBrowser->setTextColor(Qt::darkBlue);
    ui->textBrowser->setCurrentFont(QFont("微软雅黑",13));
    ui->textBrowser->append( "["+meName+"]" + "("+meId+")" + time);
    ui->textBrowser->setCurrentFont(QFont("幼圆",12));
    ui->textBrowser->append(message);
    ui->send->setEnabled(false);
    sendMessage(Message);
    ui->textEdit->clear();
    ui->textEdit->setFocus();
}
// 打开文件传输窗口
void chatDlg::on_sendfile_clicked()
{
    server->initServer();
    server->show();
    sendMessage(FileName);
//    Client *cl = new Client(this);
//    cl->show();

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
        QString fileName = QFileDialog::getSaveFileName(this, tr("保存为"), QString("%1-%2").arg(meId).arg(userId),//将用户id和好友id作为文件名
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

// 查看详细信息
void chatDlg::on_checkInfo_clicked()
{
    if(chatType == "0"){
        // 好友信息
        friendManage *fm = new friendManage(this);
        connect(fm, SIGNAL(deleteFriend()), this, SLOT(hasDel()));
        fm->setMeId(meId, userId);
        fm->initInfo();
        fm->show();
    }else if(chatType == "1"){
        // 群聊信息
        groupManage *gm = new groupManage(this);
        connect(gm, SIGNAL(quitGroup()), this, SLOT(hasDel()));
        gm->setId(meId, userId);
        gm->initInfo();
        gm->show();
    }
}
void chatDlg::hasDel(){
    this->on_close_clicked();
}

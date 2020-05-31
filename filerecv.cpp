#include "filerecv.h"
#include "ui_filerecv.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

fileRecv::fileRecv(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fileRecv)
{
    ui->setupUi(this);
    this->setFixedSize(300,200);
    TotalBytes = 0;
    bytesReceived = 0;
    fileNameSize = 0;

    tcpClient = new QTcpSocket(this);
    tcpPort = 6667;

    connect(tcpClient,SIGNAL(readyRead()),this,SLOT(readMessage()));
    connect(tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,
            SLOT(displayError(QAbstractSocket::SocketError)));
}

fileRecv::~fileRecv()
{
    delete ui;
}
void fileRecv::setHostAddress(QHostAddress address)
{
    hostAddress = address;
    newConnect();
}
// 创建一个新tcp连接
void fileRecv::newConnect()
{
    blockSize = 0;
    tcpClient->abort();
    // 在这里hostAddress使用168.254这个地址会出现host unreachable错，而127.0.0.1不会
    // 是获取IP地址出了问题
    tcpClient->connectToHost(hostAddress.toString(), tcpPort);
    connect(tcpClient,SIGNAL(readyRead()),this,SLOT(readMessage()));
    ui->quit->setEnabled(false);
    time.start();
}
// 读数据
void fileRecv::readMessage()
{
    QDataStream in(tcpClient);
    qDebug()<<"tcpClient->isReadable(): "<<tcpClient->isReadable()<<endl;
    in.setVersion(QDataStream::Qt_5_5);

    float useTime = time.elapsed();

    qDebug()<<"fileRecv::readMessage useTime:"<<useTime<<endl;

    if(bytesReceived <= sizeof(qint64)*2){
        if((tcpClient->bytesAvailable() >= sizeof(qint64)*2) && (fileNameSize == 0)){
            in>>TotalBytes>>fileNameSize;
            bytesReceived += sizeof(qint64)*2;
        }
        if((tcpClient->bytesAvailable() >= fileNameSize) && (fileNameSize != 0)){
            in>>fileName;
            bytesReceived +=fileNameSize;

            if(!localFile->open(QFile::WriteOnly)){
                QMessageBox::warning(this, QString("警告"), QString("写入错误 %1:\n%2.").arg(fileName).arg(localFile->errorString()));
                return;
            }
        }else{
            return;
        }
    }

    if(bytesReceived < TotalBytes){
        bytesReceived += tcpClient->bytesAvailable();
        inBlock = tcpClient->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);
    }

    ui->progressBar->setMaximum(TotalBytes);
    ui->progressBar->setValue(bytesReceived);
    qDebug()<<bytesReceived<<"received"<<TotalBytes;

    double speed = bytesReceived / useTime;
    ui->label->setText(QString("已接收: %1MB (%2MB/s) \n总共: %3MB 用时: %4\n剩余时间: %5")
                                   .arg(bytesReceived / (1024*1024))
                                   .arg(speed*1000/(1024*1024),0,'f',2)
                                   .arg(TotalBytes / (1024 * 1024))
                                   .arg(useTime/1000,0,'f',0)
                                   .arg(TotalBytes/speed/1000 - useTime/1000,0,'f',0));

    if(bytesReceived == TotalBytes){
        tcpClient->close();
        ui->label->setText(QString("文件%1 传输完成").arg(fileName));
        ui->quit->setEnabled(true);
        localFile->close();
    }
}
// 打印错误
void fileRecv::displayError(QAbstractSocket::SocketError socketError)
{
    switch(socketError)
    {
        case QAbstractSocket::RemoteHostClosedError : break;
        default : qDebug() <<"fileRecv::displayError " <<tcpClient->errorString();
    }
}
// 退出接收窗口
void fileRecv::on_quit_clicked()
{
    tcpClient->abort();
    this->close();
}
// 接收途中取消接收
void fileRecv::on_cancel_clicked()
{
    ui->cancel->setEnabled(false);
    ui->quit->setEnabled(true);
    tcpClient->abort();
    qDebug()<<hostAddress.toString()<<endl;
    // 发送拒收信号给chatDlg广播
    emit cancelRecv(hostAddress.toString());
}

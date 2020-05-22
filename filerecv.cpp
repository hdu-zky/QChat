#include "filerecv.h"
#include "ui_filerecv.h"
#include <QFileDialog>
#include <QMessageBox>

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
    tcpPort = 6666;

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

void fileRecv::newConnect()
{
    blockSize = 0;
    tcpClient->abort();
    tcpClient->connectToHost(hostAddress, tcpPort);
    time.start();
}

void fileRecv::readMessage()
{
    QDataStream in(tcpClient);
    in.setVersion(QDataStream::Qt_5_5);

    float useTime = time.elapsed();

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
        localFile->close();
    }
}

void fileRecv::displayError(QAbstractSocket::SocketError socketError)
{
    switch(socketError)
    {
        case QAbstractSocket::RemoteHostClosedError : break;
        default : qDebug() << tcpClient->errorString();
    }
}
void fileRecv::on_quit_clicked()
{
    tcpClient->abort();
    this->close();
}

void fileRecv::on_cancel_clicked()
{
    tcpClient->abort();
}

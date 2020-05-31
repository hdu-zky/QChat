#include "filetrans.h"
#include "ui_filetrans.h"

#include <QFileDialog>
#include <QTcpSocket>
#include <QMessageBox>
#include <QDebug>

fileTrans::fileTrans(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fileTrans)
{
    ui->setupUi(this);
    this->setFixedSize(300,200);
    tcpPort = 6667;
    tcpServer = new QTcpServer(this);
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(sendMessage()));
}
void fileTrans::initServer()
{
    loadSize = 4*1024;
    TotalBytes = 0;
//    bytesWritten = 0;
    bytesToWrite = 0;
    ui->progressBar->setValue(0);
//    ui->progressBar->->reset();
    ui->openFile->setEnabled(true);
    ui->startSend->setEnabled(false);

    tcpServer->close();

}
//
void fileTrans::sendMessage()
{

    clientConnection = tcpServer->nextPendingConnection();
    // 更新发送进度条
    connect(clientConnection,SIGNAL(bytesWritten(qint64)),SLOT(updateClientProgress(qint64)));

    ui->label->setText(QString("正在发送 %1 文件").arg(fileName));
    localFile = new QFile(filePath);
    if(!localFile->open((QFile::ReadOnly))){
        QMessageBox::warning(this, QString("警告"), QString("文件只读 %1:\n%2").arg(fileName).arg(localFile->errorString()));
        return;
    }
    TotalBytes = localFile->size();
    qDebug()<<"\nTotalBytes "<<TotalBytes<<endl;
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_5);
    time.start();
    QString currentFile = filePath.right(filePath.size() - filePath.lastIndexOf('/')-1);
    qDebug() <<"fileTrans::sendMessage "<< currentFile<<endl;
    sendOut << qint64(0) << qint64(0) << currentFile;
    TotalBytes += outBlock.size();
    sendOut.device()->seek(0);
    sendOut << TotalBytes << qint64((outBlock.size()-sizeof(qint64)*2));
    bytesToWrite = TotalBytes - clientConnection->write(outBlock);
    qDebug() << "TotalBytes "<< TotalBytes<<endl;
    outBlock.resize(0);
}

void fileTrans::updateClientProgress(qint64 numBytes)
{
    bytesWritten += (int)numBytes;

    if(bytesToWrite > 0){
        outBlock = localFile->read(qMin(bytesToWrite,loadSize));
        bytesToWrite -= (int)clientConnection->write(outBlock);
        outBlock.resize(0);
    }else{
        localFile->close();
    }

    ui->progressBar->setMaximum(TotalBytes);
    ui->progressBar->setValue(bytesWritten);

   float useTime = time.elapsed();
   double speed = bytesWritten / useTime;
   ui->label->setText(QString("已发送: %1MB (%2MB/s) \n总共: %3MB 用时: %4\n 剩余时间: %5")
                                  .arg(bytesWritten / (1024*1024))
                                  .arg(speed*1000/(1024*1024),0,'f',2)
                                  .arg(TotalBytes / (1024 * 1024))
                                  .arg(useTime/1000,0,'f',0)
                                  .arg(TotalBytes/speed/1000 - useTime/1000,0,'f',0));

   //num.sprintf("%.1f KB/s", (bytesWritten*1000) / (1024.0*time.elapsed()));
    if(bytesWritten == TotalBytes){
        ui->label->setText(QString("文件 %1 传输完成").arg(fileName));
    }
}
void fileTrans::refused()
{
    tcpServer->close();
    ui->label->setText(QString("对方已拒收"));
    ui->openFile->setEnabled(false);
    ui->startSend->setEnabled(false);
}

// 打开本地文件
void fileTrans::on_openFile_clicked()
{
    filePath = QFileDialog::getOpenFileName(this, "请选择你要传送的文件", "", "所有类型(*.*)");
    if (filePath.isEmpty()){
        QMessageBox::warning(this, "提示", "未选择数据文件", "确定");
        return;
    }
    fileName = filePath.right(filePath.size() - filePath.lastIndexOf('/')-1);
//    QFileInfo fileInfo = QFileInfo(filePath);
//    fileName = fileInfo.fileName();
//    QString absolutePath = fileInfo.absolutePath();
    ui->label->setText(QString("你已选择文件：%1").arg(fileName));
    ui->startSend->setEnabled(true);
    ui->openFile->setEnabled(false);
}
// 开始发送文件
void fileTrans::on_startSend_clicked()
{
    ui->startSend->setEnabled(false);// 正在发送中，禁止开始发送按钮
    if(!tcpServer->listen(QHostAddress::Any,tcpPort)){
        qDebug() <<"fileTrans::on_startSend_clicked error"<< tcpServer->errorString();
        tcpServer->close();
        close();
        return;
    }
    ui->label->setText(QString("文件正在发送中..."));
    // 发送文件正在发送信号给chatDlg,使他广播给目的主机窗口
    emit sendFileName(fileName);
}
//退出发送界面
void fileTrans::on_quit_clicked()
{
//    if(tcpServer->isListening())
//    {
//        tcpServer->close();
//        clientConnection->abort();
//    }
    this->close();
}
fileTrans::~fileTrans()
{
    delete ui;
}

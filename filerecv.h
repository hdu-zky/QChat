#ifndef FILERECV_H
#define FILERECV_H

#include <QFile>
#include <QTime>
#include <QDialog>
#include <QTcpSocket>
#include <QHostAddress>

namespace Ui {
class fileRecv;
}

class fileRecv : public QDialog
{
    Q_OBJECT

public:
    explicit fileRecv(QWidget *parent = 0);
    ~fileRecv();

    void setHostAddress(QHostAddress address);
    void setFileName(QString fileName){localFile = new QFile(fileName);}

private slots:
    void on_quit_clicked();
    void on_cancel_clicked();
    void newConnect();
    void readMessage();
    void displayError(QAbstractSocket::SocketError);


private:
    Ui::fileRecv *ui;
    QTcpSocket *tcpClient;
    quint16 blockSize;
    QHostAddress hostAddress;
    qint16 tcpPort;

    qint64 TotalBytes; // 文件总字节数
    qint64 bytesReceived; // 已发送的字节数
    qint64 bytesToReceive; // 尚未发送的字节数
    qint64 fileNameSize; // 文件名字字节数
    QString fileName;
    QFile *localFile;
    // 数据缓冲区
    QByteArray inBlock;

    QTime time;
};

#endif // FILERECV_H

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

    qint64 TotalBytes;
    qint64 bytesReceived;
    qint64 bytesToReceive;
    qint64 fileNameSize;
    QString fileName;
    QFile *localFile;
    QByteArray inBlock;

    QTime time;
};

#endif // FILERECV_H

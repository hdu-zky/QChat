#ifndef FILETRANS_H
#define FILETRANS_H

#include <QDialog>
#include <QTcpServer>
#include <QFile>
#include <QTime>

namespace Ui {
class fileTrans;
}

class fileTrans : public QDialog
{
    Q_OBJECT

public:
    explicit fileTrans(QWidget *parent = 0);
    ~fileTrans();
    void refused();
    void initServer();

private slots:
    void on_openFile_clicked();

    void on_startSend_clicked();

    void on_quit_clicked();
    void sendMessage();
    void updateClientProgress(qint64 numBytes);

signals:
    void sendFileName(QString fileName);
private:
    Ui::fileTrans *ui;
    qint16 tcpPort;
    QTcpServer *tcpServer;
    QString filePath, fileName;
    QFile *localFile;

    qint64 TotalBytes;
    qint64 bytesWritten;
    qint64 bytesToWrite;
    qint64 loadSize;
    QByteArray outBlock;

    QTcpSocket *clientConnection;

    QTime time;
};

#endif // FILETRANS_H

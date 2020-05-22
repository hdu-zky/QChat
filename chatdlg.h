#ifndef CHATDLG_H
#define CHATDLG_H

#include"user.h"
#include"filetrans.h"
#include"filerecv.h"

#include <QWidget>
#include <QtNetwork>
#include <QtGui>

namespace Ui {
class chatDlg;
}

enum MessageType{Message,NewParticipant,ParticipantLeft,FileName,Refuse};
class chatDlg : public QWidget
{
    Q_OBJECT

public:
    explicit chatDlg(QWidget *parent = 0);
    ~chatDlg();
    void setUId(QString uid){
        userId = uid;
    }
    void setUserName(QString uname){
        userName = uname;
    }
    void setUserInfo(QString uid, QString uname);
    QString getUserName();
    QString getMessage();
protected:
    void sendMessage(MessageType type,QString serverAddress="");
    void hasPendingFile(QString userName,QString serverAddress,
                        QString clientAddress,QString fileName);
signals:
    void closeSignal();
private slots:
    void enableSendBtn();
    void processPendingDatagrams();
    void sentFileName(QString);
    void on_send_clicked();
    void on_close_clicked();
    void on_textbold_clicked(bool checked);
    void on_textitalic_clicked(bool checked);
    void on_textUnderline_clicked(bool checked);
    void on_textcolor_clicked();
    void on_sendfile_clicked();
    void on_save_clicked();
    void on_clear_clicked();
    void on_font_currentFontChanged(const QFont &f);
    void on_fontSize_currentIndexChanged(const QString &arg1);

private:
    Ui::chatDlg *ui;
    int rawCount;
    QString userId, userName;// 保存当前对话窗口的

    QUdpSocket *udpSocket;
    qint16 port;
    QString fileName;
    fileTrans *server;
    QString getIP();
    QColor color;
    bool saveFile(const QString& fileName);
};

#endif // CHATDLG_H

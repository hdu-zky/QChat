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
    void setUserInfo(QString uid, QString uname, QString userImg);
    QString getUserName();
    QString getMessage();
protected:
    // 发送数据信息到局域网
    void sendMessage(MessageType type,QString serverAddress="");
    // 有文件数据到达
    void hasPendingFile(QString userName,QString serverAddress,
                        QString clientAddress,QString fileName);
signals:
    void closeSignal();
private slots:
    // 控制发送按钮的可用
    void enableSendBtn();
    // 有数据到达时分析处理
    void processPendingDatagrams();
    // 接收文件发送端发送的文件名
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
    int rawCount; // 记录打开窗口的个数
    // 保存当前对话窗口的好友个人信息
    QString userId, userName;
    QString userimgId, useremail, usertel, userstatus, useripAddress;

    QUdpSocket *udpSocket;
    qint16 port;
    QString fileName;
    fileTrans *server;
    QString getIP();
    QColor color;
    bool saveFile(const QString& fileName);
};

#endif // CHATDLG_H

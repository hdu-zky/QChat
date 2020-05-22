#ifndef CHATDLG_H
#define CHATDLG_H

#include <QWidget>

namespace Ui {
class chatDlg;
}

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
signals:
    void closeSignal();
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void enableSendBtn();
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
    QColor color;
    QString userId, userName;// 保存当前对话窗口的
};

#endif // CHATDLG_H

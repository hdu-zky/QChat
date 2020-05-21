#ifndef CHAT_H
#define CHAT_H

#include <QDialog>
//#include <QStandardItemModel>

class QListWidget;
namespace Ui {
class chat;
}

class chat : public QDialog
{
    Q_OBJECT

public:
    explicit chat(QWidget *parent = 0);
    ~chat();
    void setUId(QString uid){
        userId = uid;
    }
    void setUserName(QString uname){
        userName = uname;
    }

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
    Ui::chat *ui;
//    QStandardItemModel *session;
    int rawCount;
    QColor color;
    QString userId, userName;
};

#endif // CHAT_H

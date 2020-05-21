#ifndef FILETRANS_H
#define FILETRANS_H

#include <QDialog>

namespace Ui {
class fileTrans;
}

class fileTrans : public QDialog
{
    Q_OBJECT

public:
    explicit fileTrans(QWidget *parent = 0);
    ~fileTrans();

private slots:
    void on_openFile_clicked();

private:
    Ui::fileTrans *ui;
};

#endif // FILETRANS_H

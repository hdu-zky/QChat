#ifndef LIST_H
#define LIST_H

#include <QDialog>

namespace Ui {
class List;
}

class List : public QDialog
{
    Q_OBJECT

public:
    explicit List(QWidget *parent = 0);
    ~List();

private:
    Ui::List *ui;
};

#endif // LIST_H

#ifndef CONNECTTOSQL_H
#define CONNECTTOSQL_H
#include <QSqlDatabase>
#include <QSqlQuery>

class ConnectToSql {
public:
    QSqlDatabase *dataBase = new QSqlDatabase;
    ConnectToSql(){
        *dataBase=QSqlDatabase::addDatabase("QMYSQL3");
        (*dataBase).setHostName("localhost");
        (*dataBase).setUserName("root");
        (*dataBase).setPassword("223412");
        (*dataBase).setDatabaseName("test");
        (*dataBase).open();
    }
    ~ConnectToSql(){
        delete dataBase;
    }
private:

};
#endif // CONNECTTOSQL_H

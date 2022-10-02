#ifndef DATABASE_H
#define DATABASE_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSql>
#include <QSqlError>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlTableModel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QString>
#include <QVariant>
#include <QVector>
//#include <QAxObject>

class DB
{
    public:
        QSqlDatabase dataBase;
        QSqlTableModel info;

        bool connectDB (QString path);

        bool connected();

        QString analitic();

        bool createDB (QString path);

        void initTables();

        void insertStats (QJsonObject, QString);

        void showView (QString &q, QTableView *tv);

        void close();

        void exec (QString);


        DB();

        ~DB();
};

#endif // DATABASE_H

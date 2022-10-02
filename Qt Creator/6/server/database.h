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

        QVector<QString> getListPar (QString);

        bool createDB (QString path);

        void initTables();

        void createReport (QVector<QString> &);

        void insertInfo (QJsonArray arr, QPair<QString, int> &agent);

        void showView (QString &q, QTableView *tv);

        void createWord (QString );

        void close();

        void exec (QString);


        DB();

        ~DB();
};

#endif // DATABASE_H

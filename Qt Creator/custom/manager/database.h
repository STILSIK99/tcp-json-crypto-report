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

class DB
{
    public:
        QSqlDatabase dataBase;
        QSqlTableModel info;
        QVector <QString> paramsNames;

        bool connectDB (QString path);

        bool connected();

        bool createDB (QString path);

        void initTables();

        void insertResult (QJsonObject obj, QPair<QString, int> &agent);

        void showView (QString &q, QTableView *);
        void getReport (QString &q);

        void close();

        DB();

        ~DB();
};

#endif // DATABASE_H

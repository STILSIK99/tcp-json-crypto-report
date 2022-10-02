#ifndef DATABASE_H
#define DATABASE_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSql>
#include <QSqlError>
#include <QDebug>
#include <QJsonObject>
#include <QSqlTableModel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QString>
#include <QVariant>
#include <QAxObject>

class DB
{
    public:
        QSqlDatabase dataBase;
        QSqlTableModel info;
        QVector <QString> paramsNames;
        bool connectDB (QString path);

        bool connected();

        bool checkStructure ();

        bool createDB (QString path);

        void initTables();
        void showAgents (QTableWidget *tw);
        void createReport (QVector<QString> &);
        void addPackege (QJsonObject, QString ip);
        void showView (QString &q, QTableView *tv);
        void showEtalons (QTableWidget *tw);
        bool changeEtalon (QVariant &, QString &);
        void close();
        DB();
        ~DB();
};

#endif // DATABASE_H

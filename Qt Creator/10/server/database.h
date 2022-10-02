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
        QVector <QString> paramsNames;

        bool connectDB (QString path);

        bool connected();

        bool createDB (QString path);

        void initTables();

        void createReport (QVector<QString> &);

        void insertScan (QJsonObject &obj, QPair<QString, int> &agent);

        void insertResult (QJsonObject obj, int);

        void showView (QString &q, QTableView *tv);

        void createWord (QString );

        void close();

        DB();

        ~DB();
};

#endif // DATABASE_H

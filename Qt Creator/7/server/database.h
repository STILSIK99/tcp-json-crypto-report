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
#include <QMap>
//#include <QAxObject>

class DB
{
    public:
        QSqlDatabase dataBase;
        QSqlTableModel info;
        QList <QString> paramsNames;
        QMap<QString, QString> etalons;

        bool connectDB (QString path);

        bool connected();

        bool createDB (QString path);

        void initTables();

        void createReport (QVector<QString> &);

        void insertScan (QJsonObject &obj, QPair<QString, int> &agent);

        void insertResult (QJsonObject obj, int);

        QMap<QList<QString>, bool> showView (QString &q, QTableView *tv, bool);

        void close();

        bool changeEtalon (int col, QString val);

        void showEtalon (QTableView *);

        void loadEtalon();

        QString getAnalitic();

        DB();

        ~DB();
};

#endif // DATABASE_H

#include "database.h"
#include <QString>
#include <QAxObject>

void DB::initTables()
{
    QVector<QString> listTables = {
        "create table packs ( "
        " agent text, "
        " p1 int,"
        " p2 int,"
        " p3 int,"
        " date datetime)"
    };
    QSqlQuery query;
    for (int i = 0; i < listTables.size(); ++i) {
        if (!query.exec (listTables[i])) {
            qDebug() << "DataBase: error of create table.";
            qDebug() << query.lastError().text();
        }
    }
}

void DB::exec (QString q)
{
    QSqlQuery sql;
    sql.exec (q);
}

void DB::insertStats (QJsonObject obj, QString agent)
{
    QVector<QString> names = {
        "Number of received datagrams",
        "Number of datagrams forwarded",
        "Number of outgoing datagrams requested to transmit"
    };
    if (!connected()) return;
    QString q = "insert into packs values (?, ?, ?, ?, CURRENT_TIMESTAMP);";
    QSqlQuery query;
    query.prepare (q);
    query.bindValue (0, agent);
    for (int i = 0; i < 3; ++i) {
        if (!obj.contains (names[i])) return;
        query.bindValue (i + 1, obj[names[i]].toInt());
    }
    if (!query.exec ()) {
        qDebug() << query.lastError().text() << "\n";
        qDebug() << query.executedQuery();
        return;
    }
}

QString DB::analitic()
{
    return "";
}

bool DB::createDB (QString path)
{
    try {
        dataBase = dataBase.addDatabase ("QSQLITE");
        dataBase.setDatabaseName (path);
        dataBase.open();
        initTables();
        return true;
    } catch (...) {
        return false;
    }
}

bool DB::connected()
{
    return dataBase.isOpen();
};

bool DB::connectDB (QString path)
{
    dataBase = QSqlDatabase::addDatabase ("QSQLITE");
    dataBase.setDatabaseName (path);
    if (dataBase.open()) {
        return true;
    }
    return false;
};

void DB::showView (QString &q, QTableView *tv)
{
    if (!connected()) return;
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery (q);
    tv->setModel (model);
    tv->resizeColumnsToContents();
}

void DB::close()
{
    if (dataBase.isOpen()) dataBase.close();
};

DB::DB() {};

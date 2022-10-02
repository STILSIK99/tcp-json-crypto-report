#include "database.h"


void DB::initTables()
{
    QVector<QString> listTables = {
        "create table scan( "
        "id integer primary key AUTOINCREMENT,"
        "agent text, "
        "dir text, "
        "reg text "
        ")",
        "create table results("
        "    id integer primary key AUTOINCREMENT,"
        "    id_search integer REFERENCES search(id),"
        "    file text,"
        "    count int"
        ")"
    };
    QSqlQuery query;
    for (int i = 0; i < listTables.size(); ++i) {
        if (!query.exec (listTables[i])) {
            qDebug() << "DataBase: error of create table.";
            qDebug() << query.lastError().text();
        }
    }
}

void DB::insertScan (QJsonObject &obj, QPair<QString, int> &agent)
{
    if (!connected()) return;
    QString q = "insert into scan values (null, ?, ?, ?);";
    QSqlQuery query;
    query.prepare (q);
    query.bindValue (0, agent.first);
    query.bindValue (1, obj["dir"].toString());
    query.bindValue (2, obj["reg"].toString());
    if (!query.exec ()) {
        qDebug() << query.lastError().text() << "\n";
        qDebug() << query.executedQuery();
        return;
    }
    if (!obj.contains ("results")) return;
    int id_search = -1;
    q = "select id from scan "
        "where agent = ? and "
        "dir = ? and reg = ?";
    query.prepare (q);
    query.bindValue (0, agent.first);
    query.bindValue (1, obj["dir"].toString());
    query.bindValue (2, obj["reg"].toString());
    if (!query.exec ()) {
        qDebug() << query.lastError().text() << "\n";
        qDebug() << query.executedQuery();
        return;
    }
    if (!query.next()) {
        return;
    }
    id_search = query.value (0).toInt();
    QJsonArray masFile = obj["results"].toArray();
    for (int i = 0; i < masFile.size(); ++i) {
        insertResult (masFile[i].toObject(), id_search);
    }

}

void DB::insertResult (QJsonObject obj, int id)
{
    QString q = "insert into results values (null, ?, ?, ?)";
    QSqlQuery query;
    query.prepare (q);
    query.bindValue (0, id);
    query.bindValue (1, obj["file"].toString());
    query.bindValue (2, obj["count"].toInt());
    if (!query.exec()) {
        qDebug() << query.lastError().text() << "\n";
        qDebug() << query.executedQuery();
    }
};

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

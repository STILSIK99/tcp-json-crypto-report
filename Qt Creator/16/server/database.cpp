#include "database.h"
#include <QAxObject>
#include <QTableWidget>
#include <QTableWidgetItem>

void DB::initTables()
{
    QVector<QString> listTables = {
        "create table reads( "
        "id_r integer primary key AUTOINCREMENT, "
        "agent text, "
        "file text, "
        "date datetime "
        "); ",
        "create table blocks( "
        "id_r integer REFERENCES reads(id_r) ON DELETE CASCADE,"
        "block text, "
        "num integer"
        ");"
    };
    QSqlQuery query;
    for (int i = 0; i < listTables.size(); ++i) {
        if (!query.exec (listTables[i])) {
            qDebug() << "DataBase: error of create table.";
            qDebug() << query.lastError().text();
        }
    }
}

QString DB::getBlock (QString ag, QString file, int block)
{
    QString res = "";
    QString q = QString ("select b.block from blocks b, reads r "
                         "where r.id_r = b.id_r and r.agent = '%1' and r.file = '%2' "
                         " and b.num = %3").arg (ag, file, QString::number (block));
    QSqlQuery sql;
    if (!sql.exec (q)) {
        qDebug() << sql.lastQuery();
        qDebug() << sql.lastError().text();
    } else {
        if (sql.first())
            res = sql.value (0).toString();
    }
    return res;
};

void DB::insertRead (QJsonObject &obj, QPair<QString, int> &agent)
{
    qDebug() << agent.first << " " << obj["file"].toString();
    if (!connected()) return;
    QString q = QString ("delete from reads "
                         "where agent = '%1' and file = '%2'").arg (
                    agent.first,
                    obj["file"].toString());
    QSqlQuery query;
    if (!query.exec (q)) {
        qDebug() << query.lastError().text() << "\n";
        qDebug() << query.executedQuery();
        return;
    }
    q = QString ("insert into reads values (null, '%1', '%2', CURRENT_TIMESTAMP);").arg (agent.first, obj["file"].toString());
    if (!query.exec (q)) {
        qDebug() << query.lastError().text() << "\n";
        qDebug() << query.executedQuery();
        return;
    }
    q = "select max(id_r) from reads";
    int id = 1;
    if (!query.exec (q)) {
        qDebug() << query.lastError().text() << "\n";
        qDebug() << query.executedQuery();
        return;
    }
    if (query.first()) {
        id = query.value (0).toInt();
    }
    QJsonArray mas = obj["blocks"].toArray();
    for (int i = 0; i < mas.size(); ++i) {
        if (!query.exec (
                QString ("insert into blocks values (%1, '%2', %3)").arg (
                    QString::number (id),
                    mas[i].toString(),
                    QString::number (i + 1)))) {
            qDebug() << query.lastError().text() << "\n";
            qDebug() << query.executedQuery();
        }
    }
}

int DB::getMaxBlock (QString ag, QString file)
{
    QString q = "select max(num) from reads r, blocks b "
                "where r.id_r = b.id_r and r.agent = ? "
                "and r.file = ? ";
    QSqlQuery sql;
    sql.prepare (q);
    sql.bindValue (0, ag);
    sql.bindValue (1, file);
    if (!sql.exec()) {
        qDebug() << sql.lastError().text() << "\n";
        qDebug() << sql.executedQuery();
        return 1;
    }
    if (!sql.first()) {
        return 1;
    }
    return sql.value (0).toInt();
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

void DB::showView (QString &q, QTableWidget *tw)
{
    if (!connected()) return;
    QSqlQuery sql;
    tw->clear();
    tw->setRowCount (0);
    tw->setColumnCount (3);
    tw->setHorizontalHeaderLabels ({"Агент", "Файл", "Дата просмотра"});
    if (!sql.exec (q)) {
        return;
    }
    while (sql.next()) {
        int row = tw->rowCount();
        tw->setRowCount (row + 1);
        for (int i = 0; i < 3; ++i) {
            tw->setItem (row, i, new QTableWidgetItem (sql.value (i).toString()));
        }
    }
}

void DB::close()
{
    if (dataBase.isOpen()) dataBase.close();
};

DB::DB() {};


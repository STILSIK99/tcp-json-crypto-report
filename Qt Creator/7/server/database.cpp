#include "database.h"


void DB::initTables()
{
    QVector<QString> listTables = {
        "create table scan( "
        "agent text, "
        "par1 text, "
        "par2 text, "
        "par3 text, "
        "par4 text, "
        "par5 text, "
        "par6 text, "
        "par7 text, "
        "par8 text "
        ")",
        "create table etalon("
        "par1 text, "
        "par2 text, "
        "par3 text, "
        "par4 text, "
        "par5 text, "
        "par6 text, "
        "par7 text, "
        "par8 text "
        ")"
    };
    QSqlQuery query;
    for (int i = 0; i < listTables.size(); ++i) {
        if (!query.exec (listTables[i])) {
            qDebug() << "DataBase: error of create table.";
            qDebug() << query.lastError().text();
        }
    }
    query.exec ("insert into etalon values ('', '', '', '', '', '', '', '')");
}

void DB::insertScan (QJsonObject &obj, QPair<QString, int> &agent)
{
    if (!connected()) return;
    QString qq = "delete from scan where agent = ?";
    QSqlQuery query;
    query.prepare (qq);
    query.bindValue (0, agent.first);
    if (!query.exec()) {
        qDebug() << query.lastError().text() << "\n";
        qDebug() << query.executedQuery();
        return;
    }
    if (!obj.contains ("result")) return;
    auto res = obj["result"].toObject();
    QString q = "insert into scan values (?, ?, ?, ?, ?, ?, ?, ?, ?);";
    query.prepare (q);
    query.bindValue (0, agent.first);
    for (int i = 0; i < 8; ++i) {
        if (res.contains (paramsNames[i])) {
            query.bindValue (i + 1, res[paramsNames[i]].toString());
        } else query.bindValue (i + 1, "");
    }
    if (!query.exec ()) {
        qDebug() << query.lastQuery();
        qDebug() << query.lastError().text() << "\n";
        qDebug() << query.executedQuery();
        return;
    }
}

bool DB::changeEtalon (int col, QString val)
{
    QString q = QString ("update etalon "
                         "set par%1 = '%2'").arg (QString::number (col + 1), val);
    QSqlQuery query;
    if (!query.exec (q)) {
        qDebug() << "DataBase: error of create table.";
        qDebug() << q;
        qDebug() << query.lastError().text();
        return false;
    }
    loadEtalon();
    return true;
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


QString DB::getAnalitic()
{
    loadEtalon();
    QString agent = "";
    int count = 0;
    QString q = "select par1,par2,par3,par4,par5,par6,par7, agent from scan";
    QSqlQuery sql;
    if (!sql.exec (q)) {
        return "Not found";
    }
    while (sql.next()) {
        int p = 0;
        for (int i = 0; i < 7; ++i) {
            if (etalons[paramsNames[i]] != sql.value (i).toString())
                ++p;
        }
        if (p > count) agent = sql.value (7).toString();
    }
    return agent;
}

void DB::loadEtalon()
{
    if (!connected()) return;
    QString q = "select par1, par2, par3, par4, par5, par6, par7, par8 from etalon";
    QSqlQuery query;
    if (!query.exec (q)) return;
    if (!query.first()) return;
    for (int i = 0; i < 8; ++i) {
        etalons[paramsNames[i]] = query.value (i).toString();
    }
}

QMap<QList<QString>, bool> DB::showView (QString &add, QTableView *tv, bool fl)
{
    QMap<QList<QString>, bool> res;
    QString q = QString ("select agent as 'Agent', par1 as '%1',  par2 as '%2', "
                         "par3 as '%3', par4 as '%4', "
                         "par5 as '%5', par6 as '%6', "
                         "par7 as '%7', par8 as '%8' "
                         "from scan").arg (
                    paramsNames[0], paramsNames[1],
                    paramsNames[2], paramsNames[3],
                    paramsNames[4], paramsNames[5],
                    paramsNames[6], paramsNames[7]);
    q += add;
    if (!connected()) return res;
    QSqlQueryModel *model = new QSqlQueryModel;
    qDebug() << q;
    model->setQuery (q);
    tv->setModel (model);
    tv->resizeColumnsToContents();
    if (fl) {
        QSqlQuery query;
        if (!query.exec (q)) return res;
        while (query.next()) {
            QList<QString> line;
            bool fl = true;
            line.append (query.value (0).toString());
            for (int i = 1; i < 9; ++i) {
                line.append (query.value (i).toString());
                if (line[i] != etalons[paramsNames[i - 1]])
                    fl = false;
            }
            res[line] = fl;
        }
    }
    return res;
}

void DB::showEtalon (QTableView *tv)
{
    if (!connected()) return;
    QString q = QString ("select par1 as '%1',  par2 as '%2', "
                         "par3 as '%3', par4 as '%4', "
                         "par5 as '%5', par6 as '%6', "
                         "par7 as '%7', par8 as '%8' "
                         "from etalon").arg (
                    paramsNames[0], paramsNames[1],
                    paramsNames[2], paramsNames[3],
                    paramsNames[4], paramsNames[5],
                    paramsNames[6], paramsNames[7]);
    QSqlQueryModel *model = new QSqlQueryModel;
    qDebug() << q;
    model->setQuery (q);
    tv->setModel (model);
    tv->resizeColumnsToContents();
};

void DB::close()
{
    if (dataBase.isOpen()) dataBase.close();
};

DB::DB()
{
    paramsNames.clear();
    paramsNames.append ("Computer name");
    paramsNames.append ("User name");
    paramsNames.append ("System Directory");
    paramsNames.append ("Windows Directory");
    paramsNames.append ("OS");
    paramsNames.append ("PATH");
    paramsNames.append ("HOMEPATH");
    paramsNames.append ("TEMP");
    for (int i = 0; i < 8; ++i) {
        etalons[paramsNames[i]] = "";
    }
};

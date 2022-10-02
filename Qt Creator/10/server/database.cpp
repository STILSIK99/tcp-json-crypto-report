#include "database.h"
#include <QString>
#include <QAxObject>

void DB::initTables()
{
    QVector<QString> listTables = {
        "create table scan( "
        "   agent text, "
        "   volume text, "
        "   size int, "
        "   busy int, "
        "   free int, "
        "   date datetime "
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
    qDebug() << agent.first << " " << obj["file"].toString();
    if (!connected()) return;
    QString q = "insert into scan values (?, ?, ?, ?, ?, CURRENT_TIMESTAMP);";
    QSqlQuery query;
    query.prepare (q);
    query.bindValue (0, agent.first);
    query.bindValue (1, obj["volume"].toString());
    query.bindValue (2, obj["size"].toInt());
    query.bindValue (3, obj["size"].toInt() - obj["free"].toInt());
    query.bindValue (4, obj["free"].toInt());
    if (!query.exec ()) {
        qDebug() << query.lastError().text() << "\n";
        qDebug() << query.executedQuery();
        return;
    }
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

void DB::createWord (QString q)
{
    QSqlQuery sql;
    if (!sql.exec (q)) return;
    QAxObject *word = new QAxObject ("Word.Application");
    QAxObject *docs = word->querySubObject ("Documents");
    QAxObject *doc = docs->querySubObject ("Add()");
    QAxObject *pars = doc->querySubObject ("Paragraphs");

    QAxObject *title = pars->querySubObject ("Add()")->querySubObject ("Range()");
    title->dynamicCall ("SetRange(int, int)", 0, 50);
    title->setProperty ("Text", "Журнал изменений  объема  свободного  и  занятого  места  на  диске.\n");

    while (sql.next()) {
        QString TextStr = QString ("Агент - %1, раздел - %2, "
                                   "размер занятого пространства - %3, Время - %4.").arg (
                              sql.value (0).toString(),
                              sql.value (1).toString(),
                              QString::number (sql.value (2).toInt()),
                              sql.value (3).toString());
        auto par = pars->querySubObject ("Last");
        auto word_range = doc->querySubObject ("Content");
        par->setProperty ("Alignment", 3);
        auto range = par->querySubObject ("Range");
        range->dynamicCall ("InsertAfter(QString)", TextStr);
        word_range->dynamicCall ("InsertParagraphAfter(void)");
        delete par;
    }
    word->querySubObject ("Selection")->querySubObject ("Font")->setProperty ("Size", 12);
    word->querySubObject ("Selection")->querySubObject ("Font")->setProperty ("Name", "Times New Roman");
    word->setProperty ("Visible", true);
}

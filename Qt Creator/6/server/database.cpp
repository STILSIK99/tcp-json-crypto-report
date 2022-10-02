#include "database.h"
#include <QString>
#include <QAxObject>

void DB::initTables()
{
    QVector<QString> listTables = {
        "create table info ( "
        " agent text, "
        " p1 text,"
        " p2 text,"
        " p3 text,"
        " p4 text,"
        " p5 text,"
        " p6 text,"
        " p7 text,"
        " date datetime)",
        "insert into info values('etalon', '','','','','','','', CURRENT_TIMESTAMP)"
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

void DB::insertInfo (QJsonArray obj, QPair<QString, int> &agent)
{
    if (!connected()) return;
    exec (QString ("delete from info where agent = '%1'").arg (agent.first));
    QString q = "insert into info values (?, ?, ?, ?, ?, ?, ?, ?, CURRENT_TIMESTAMP);";
    QSqlQuery query;
    query.prepare (q);
    query.bindValue (0, agent.first);
    for (int i = 0; i < std::min (8, obj.size()); ++i) {
        if (obj[i].isDouble()) {
            query.bindValue (i + 1, QString::number (obj[i].toInt()));
        } else query.bindValue (i + 1, obj[i].toString());
    }
    if (!query.exec ()) {
        qDebug() << query.lastError().text() << "\n";
        qDebug() << query.executedQuery();
        return;
    }
}

QString DB::analitic()
{
    int max = 0;
    QString agent;
    auto etalon = getListPar ("etalon");
    QString q = "select p1, p2, p3, p4, p5, p6, p7, agent from info "
                "where agent <> 'etalon'";
    QSqlQuery sql;
    sql.exec (q);
    while (sql.next()) {
        int n = 0;
        for (int i = 0 ; i < 7; ++i) {
            if (sql.value (i).toString() != etalon[i])
                ++n;
        }
        if (n > max) {
            max = n;
            agent = sql.value (7).toString();
        }
    }
    return agent;
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

QVector<QString> DB::getListPar (QString ag)
{
    if (!connected()) return {};
    QString q = QString ("select p1,p2,p3,p4,p5,p6,p7 from info where agent = '%1'").arg (ag);
    QSqlQuery sql;
    if (!sql.exec (q)) return {};
    if (!sql.first()) return {};
    QVector<QString> list (7);
    for (int i = 0; i < 7; ++i)
        list[i] = sql.value (i).toString();
    return list;
}

void DB::close()
{
    if (dataBase.isOpen()) dataBase.close();
};

DB::DB() {};

void DB::createWord (QString q)
{
    QVector<QString> parNames ({"OEM ID",
                                "Number of processors",
                                "Page size",
                                "Processor type",
                                "Minimum application address",
                                "Maximum application address",
                                "Active processor mask"});
    auto etalons = getListPar ("etalon");

    QAxObject *word = new QAxObject ("Word.Application");
    QAxObject *docs = word->querySubObject ("Documents");
    QAxObject *doc = docs->querySubObject ("Add()");
    QAxObject *pars = doc->querySubObject ("Paragraphs");

    QAxObject *title = pars->querySubObject ("Add()")->querySubObject ("Range()");
    title->dynamicCall ("SetRange(int, int)", 0, 50);
    title->setProperty ("Text", "Обнаружение несоответствий эталону.");
    //    title->setProperty ("Size", 18);
    title->dynamicCall ("InsertParagraphAfter()");

    QSqlQuery sql;
    sql.exec (q);
    int i = 0;
    while (sql.next()) {
        pars->querySubObject ("Add()");
        QAxObject *par = pars->querySubObject ("Last");
        auto range = par->querySubObject ("Range()");
        range->setProperty ("Text", "Агент: " + sql.value (7).toString() + "\n");
        QAxObject *r = pars->querySubObject ("Add()")->querySubObject ("Range()");
        QAxObject *tables = doc->querySubObject ( "Tables" );
        QAxObject *table = tables->querySubObject ("Add(Range, NumRows, NumColomns)", r->asVariant(), 8, 3);
        table->querySubObject ( "Cell( Long, Long )", 1, 1)->querySubObject ("Range")->setProperty ("Text", "Название параметра");
        table->querySubObject ( "Cell( Long, Long )", 1, 2)->querySubObject ("Range")->setProperty ("Text", "Значение агента");
        table->querySubObject ( "Cell( Long, Long )", 1, 3)->querySubObject ("Range")->setProperty ("Text", "Эталонное значение");
        for (int j = 0; j < 7; ++j) {
            table->querySubObject ( "Cell( Long, Long )", j + 2, 1)->querySubObject ("Range")->setProperty ("Text", QVariant (parNames[j]));
            QAxObject *r = table->querySubObject ( "Cell( Long, Long )", j + 2, 2)->querySubObject ("Range");
            r->setProperty ("Text", sql.value (j));
            if (sql.value (j) != etalons[j])
                r->querySubObject ("Font")->setProperty ("ColorIndex", "wdRed");
            else
                r->querySubObject ("Font")->setProperty ("ColorIndex", "wdGreen");
            table->querySubObject ( "Cell( Long, Long )", j + 2, 3)->querySubObject ("Range")->setProperty ("Text", etalons[j]);
        }
    }
    word->querySubObject ("Selection")->querySubObject ("Font")->setProperty ("Size", 12);
    word->querySubObject ("Selection")->querySubObject ("Font")->setProperty ("Name", "Times New Roman");
    word->setProperty ("Visible", true);
}

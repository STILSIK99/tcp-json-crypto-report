#include "DataBase.h"
#include <QSqlQueryModel>
#include <QVector>
using std::vector;

DB::DB()
{
    paramsNames = {
        "OEM ID", "Number of processors", "Page size",
        "Processor type", "Minimum application address",
        "Maximum application address", "Active processor mask"
    };
}

bool DB::connectDB (QString path)
{
    dataBase = QSqlDatabase::addDatabase ("QSQLITE");
    //    db.setHostName(DATABASE_HOSTNAME);
    dataBase.setDatabaseName (path);
    if (dataBase.open()) {
        if (checkStructure ()) {
            return true;
        } else dataBase.close();
    }
    return false;
};

void DB::initTables()
{
    QSqlQuery query;
    QString q = "create table data ("
                " date datetime default CURRENT_TIMESTAMP,"
                " par1 integer, ip Text not null,"
                " par2 integer, par3 integer,"
                " par4 integer, par5 text,"
                " par6 text, par7 INTEGER);";
    if (!query.exec (q)) {
        qDebug() << "DataBase: error of create table.";
        qDebug() << query.lastError().text();
    }
    q = "create table etalons("
        "par1 integer, par2 integer,"
        "par3 integer, par4 integer,"
        "par5 text, par6 text, par7 integer);";
    if (!query.exec (q)) {
        qDebug() << "DataBase: error of create table.";
        qDebug() << query.lastError().text();
    }
    q = "insert into etalons values (0,0,0,0,\"\", \"\", 0)";
    if (!query.exec (q)) {
        qDebug() << "DataBase: error of create table.";
        qDebug() << query.lastError().text();
    }
}

bool DB::connected()
{
    return dataBase.isOpen();
};

void DB::addPackege (QJsonObject pack, QString agent)
{
    QString q = "insert into data values (\"ip\",\"date\", par1, par2, par3,"
                " par4, \"par5\", \"par6\", par7);";
    for (auto key : pack.keys()) {
        q = q.replace (key, (pack[key].isDouble()) ? QString::number (pack[key].toDouble()) : pack[key].toString());
    }
    q = q.replace ("date", pack["time"].toString()).replace ("ip", agent);
    QSqlQuery query;
    if (!query.exec (q)) {
        qDebug() << query.lastError().text() << "\n";
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

void DB::close()
{
    if (dataBase.isOpen()) dataBase.close();
};

bool DB::checkStructure ()
{
    QVector<QString> tables = {
        "data", "etalons"
    };
    QVector<QVector<QString>> fields = {
        {"ip", "date", "par1", "par2", "par3", "par4", "par5", "par6", "par7"},
        {"par1", "par2", "par3", "par4", "par5", "par6", "par7"}
    };
    for (int i = 0; i < tables.size(); ++i) {
        QString zapros = "select * from table where 0 = 1";
        QString attrs = "";
        for (auto field : fields[i]) attrs += field + ",";
        attrs[attrs.length() - 1] = ' ';
        try {
            QSqlQuery query;
            QString q = zapros.replace ("*", attrs).replace ("table", tables[i]);
            if (!query.exec (q)) {
                qDebug() << query.lastError().text() << "\n";
                return false;
            }
        } catch (...) {
            return false;
        }
    }
    return true;
}

DB::~DB() {}

void DB::showView (QString &q, QTableView *tv)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery (q);
    tv->setModel (model);
    tv->resizeColumnsToContents();
}

bool DB::changeEtalon (QVariant &d, QString &p)
{
    QSqlQuery query;
    QString q = "update etalons set ";
    q += p + " = ";
    if (p == "par5" || p == "par6")
        q += "'";
    q += d.toString();
    if (p == "par5" || p == "par6")
        q += "'";
    qDebug() << q;
    if (query.exec (q))
        return true;
    return false;
}
void DB::showEtalons (QTableWidget *tw)
{
    QSqlQuery query;
    QString q = "select * from etalons";
    if (!query.exec (q)) {
        qDebug() << "DataBase: error of create table.";
        qDebug() << query.lastError().text();
        return;
    }
    tw->clear();
    if (!query.next()) return;
    tw->setRowCount (7);
    tw->setColumnCount (2);
    for (int i = 0; i < 7; ++i) {
        QTableWidgetItem *it1 = new QTableWidgetItem (paramsNames[i]);
        it1->setFlags (it1->flags() & (~Qt::ItemIsEditable));
        tw->setItem (i, 0, it1);
        QTableWidgetItem *it2 = new QTableWidgetItem (query.value (i).toString());
        it2->setFlags (it2->flags() & (~Qt::ItemIsEditable));
        tw->setItem (i, 1, it2);
    }
    tw->setHorizontalHeaderLabels ({"Название", "Значение"});
    tw->resizeColumnsToContents();
}

void DB::showAgents (QTableWidget *tw)
{
    tw->setColumnCount (1);
    QString q = "select distinct ip from data";
    QSqlQuery query;
    if (!query.exec (q)) return;
    tw->clear();
    tw->setRowCount (0);
    while (query.next()) {
        tw->setRowCount (tw->rowCount() + 1);
        QTableWidgetItem *it = new QTableWidgetItem (query.value (0).toString());
        it->setData (Qt::CheckStateRole, Qt::Unchecked);
        tw->setItem (tw->rowCount() - 1, 0, it);
    }
    tw->setHorizontalHeaderLabels ({"Ip адрес агента"});
    tw->resizeColumnsToContents();
}

void DB::createReport (QVector<QString> &agents)
{
    QList<QVariant> etalon;
    QSqlQuery query;
    QString q;

    q = "select par1, par2, par3, par4, par5, par6, par7 from etalons";
    if (!query.exec (q)) return ;
    query.first();
    for (int i = 0; i < 7; ++i)
        etalon.push_back (query.value (i));

    QAxObject *word = new QAxObject ("Word.Application");
    QAxObject *docs = word->querySubObject ("Documents");
    QAxObject *doc = docs->querySubObject ("Add()");
    QAxObject *pars = doc->querySubObject ("Paragraphs");

    QAxObject *title = pars->querySubObject ("Add()")->querySubObject ("Range()");
    title->dynamicCall ("SetRange(int, int)", 0, 50);
    title->setProperty ("Text", "Обнаружение несоответствий эталону.");
    //    title->setProperty ("Size", 18);
    title->dynamicCall ("InsertParagraphAfter()");

    for (int i = 0; i < agents.size(); ++i) {
        q = QString ("select d.par1, d.par2, d.par3, d.par4, d.par5, d.par6, d.par7 from data d, etalons e where "
                     "d.date = (select max(data.date) from data where ip = 'agent') and "
                     " (d.par1 <> e.par1 or "
                     "  d.par2 <> e.par2 or d.par3 <> e.par3 or "
                     "  d.par4 <> e.par4 or d.par5 <> e.par5 or "
                     "  d.par6 <> e.par6 or d.par7 <> e.par7)").replace ("agent", agents[i]);
        if (!query.exec (q)) continue;
        pars->querySubObject ("Add()");
        QAxObject *par = pars->querySubObject ("Last");
        auto range = par->querySubObject ("Range()");
        range->setProperty ("Text", "Агент: " + agents[i] + "\n");
        if (!query.next()) {
            range->dynamicCall ("InsertAfter(Text)", QVariant ("Соответствует эталону.\n"));
        } else {
            QAxObject *r = pars->querySubObject ("Add()")->querySubObject ("Range()");
            QAxObject *tables = doc->querySubObject ( "Tables" );
            QAxObject *table = tables->querySubObject ("Add(Range, NumRows, NumColomns)", r->asVariant(), 8, 3);
            table->querySubObject ( "Cell( Long, Long )", 1, 1)->querySubObject ("Range")->setProperty ("Text", "Название параметра");
            table->querySubObject ( "Cell( Long, Long )", 1, 2)->querySubObject ("Range")->setProperty ("Text", "Значение агента");
            table->querySubObject ( "Cell( Long, Long )", 1, 3)->querySubObject ("Range")->setProperty ("Text", "Эталонное значение");
            for (int j = 0; j < 7; ++j) {
                table->querySubObject ( "Cell( Long, Long )", j + 2, 1)->querySubObject ("Range")->setProperty ("Text", QVariant (paramsNames[j]));
                QAxObject *r = table->querySubObject ( "Cell( Long, Long )", j + 2, 2)->querySubObject ("Range");
                r->setProperty ("Text", query.value (j));
                if (query.value (j) != etalon[j])
                    r->querySubObject ("Font")->setProperty ("ColorIndex", "wdRed");
                else
                    r->querySubObject ("Font")->setProperty ("ColorIndex", "wdGreen");
                table->querySubObject ( "Cell( Long, Long )", j + 2, 3)->querySubObject ("Range")->setProperty ("Text", etalon[j]);
            }
        }
    }
    word->querySubObject ("Selection")->querySubObject ("Font")->setProperty ("Size", 12);
    word->querySubObject ("Selection")->querySubObject ("Font")->setProperty ("Name", "Times New Roman");
    word->setProperty ("Visible", true);
};

#include "database.h"
#include <QAxObject>

void DB::initTables()
{
    QVector<QString> listTables = {
        "create table scan( "
        "   agent text, "
        "   file text, "
        "   size int, "
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
    QString q = "insert into scan values (?, ?, ?, CURRENT_TIMESTAMP);";
    QSqlQuery query;
    query.prepare (q);
    query.bindValue (0, agent.first);
    query.bindValue (1, obj["file"].toString());
    query.bindValue (2, obj["size"].toInt());
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

void DB::createExcel (QString q)
{
    QSqlQuery sql;
    if (!sql.exec (q)) return;
    QAxObject *excel = new QAxObject ("Excel.Application");
    excel->setProperty ("DisplayAlerts", "0");

    QAxObject *workbooks = excel->querySubObject ( "Workbooks" );

    QAxObject *workbook = workbooks->querySubObject ("Add()");
    QAxObject *sheets = workbook->querySubObject ( "Sheets" );
    QAxObject *sheet = sheets->querySubObject ("Item(int)", 1);

    int row = 1;
    QVector<QString> mas = {"Агент", "Файл", "Размер", "Время"};
    for (int i = 1; i < 5; ++i) {
        sheet->querySubObject ("Cells(QVariant,QVariant)", row, i)->setProperty ("Value", QVariant (mas[i - 1]));
    }
    ++row;
    while (sql.next()) {
        for (int i = 1; i < 5; ++i) {
            sheet->querySubObject ("Cells(QVariant,QVariant)", row, i)->setProperty ("Value", sql.value (i - 1));
        }
        ++row;
    }
    excel->setProperty ("Visible", true);
}

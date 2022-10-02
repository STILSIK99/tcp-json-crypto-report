#include "database.h"
#include <QAxObject>
#include <QTableWidget>
#include <QTableWidgetItem>

void DB::initTables()
{
    QVector<QString> listTables = {
        "create table history ( "
        "agent text, "
        "url text, "
        "result text, "
        "date datetime "
        "); ",
    };
    QSqlQuery query;
    for (int i = 0; i < listTables.size(); ++i) {
        if (!query.exec (listTables[i])) {
            qDebug() << "DataBase: error of create table.";
            qDebug() << query.lastError().text();
            qDebug() << query.lastQuery();
        }
    }
}


void DB::insertResult (QJsonObject obj, QPair<QString, int> &agent)
{
    QSqlQuery query;
    QString q = QString ("insert into history values ('%1', '%2', '%3', CURRENT_TIMESTAMP);").arg (
                    agent.first, obj["url"].toString(), obj["result"].toString());
    if (!query.exec (q)) {
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

void DB::getReport (QString &q)
{
    QSqlQuery query;
    if (!query.exec (q)) {
        return;
    }
    QAxObject *excel = new QAxObject ("Excel.Application");
    excel->setProperty ("DisplayAlerts", "0");

    QAxObject *workbooks = excel->querySubObject ( "Workbooks" );

    QAxObject *workbook = workbooks->querySubObject ("Add()");
    QAxObject *sheets = workbook->querySubObject ( "Sheets" );
    QAxObject *sheet = sheets->querySubObject ("Item(int)", 1);

    int row = 2;
    sheet->querySubObject (
        "Cells(QVariant,QVariant)", 1, 1)->setProperty ("Value", "Агент");
    sheet->querySubObject (
        "Cells(QVariant,QVariant)", 1, 2)->setProperty ("Value", "URL");
    sheet->querySubObject (
        "Cells(QVariant,QVariant)", 1, 3)->setProperty ("Value", "Результат проверки");
    sheet->querySubObject (
        "Cells(QVariant,QVariant)", 1, 4)->setProperty ("Value", "Дата проверки");
    while (query.next()) {
        for (int i = 0; i < 4; ++i)
            sheet->querySubObject (
                "Cells(QVariant,QVariant)",
                row,
                i + 1)->setProperty (
                    "Value",
                    query.value (i)
                );
        row++;
    }
    excel->setProperty ("Visible", true);
}


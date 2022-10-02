#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QVariant>

MainWindow::MainWindow (QWidget *parent)
    : QMainWindow (parent)
    , ui (new Ui::MainWindow)
{
    ui->setupUi (this);

    server = new Server();
    //    connect (server, &Server::operationFinished, this, &MainWindow::slotOperationFinish);
    connect (server, &Server::recievedMessage, this, &MainWindow::slotProcess);
    connect (this, &MainWindow::makeConnection, server, &Server::slotMakeConnection);
    //open
    connect (ui->action, &QAction::triggered, this, &MainWindow::slotOpenDB);
    //create
    connect (ui->action_2, &QAction::triggered, this, &MainWindow::slotCreateDB);

    db = new DB();

    ui->comboBox->clear();
    ui->comboBox->addItems (db->paramsNames);
    ui->lineEdit->setValidator (
        new QRegExpValidator (QRegExp (
                                  "^(([0-9]|[1-9][0-9]|1[0-9]{2}|"
                                  "2[0-4][0-9]|25[0-5])\\.){3}([0-9]"
                                  "|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]"
                                  "|25[0-5])$")
                             )
    );
    ui->lineEdit_2->setValidator ( new QIntValidator (1000, 65000, this) );
    //    ui->tabWidget->setEnabled (true);
}

void MainWindow::slotOpenDB()
{
    const QString title = "Подключение БД.";
    if (db->connected()) {
        auto x = QMessageBox::question (this, title, "Подлкючить новую БД?");
        if (x == QMessageBox::Yes) {
            db->close();
        } else return;
    }
    QString path = QFileDialog::getOpenFileName (this, "Выберите файл базы данных:", "/", "DataBase (*.db)");
    if (path == "") return;
    if (db->connectDB (path)) {
        QMessageBox::about (this, title, "Успешно.");
        db->showEtalon (ui->tableView_2);
        ui->tabWidget->setEnabled (true);
    } else {
        QMessageBox::about (this, title, "Не подключено.");
    }
}

void MainWindow::slotCreateDB()
{
    const QString title = "Создание БД.";
    if (db->connected()) {
        auto x = QMessageBox::question (this, title, "Подлкючить новую БД?");
        if (x == QMessageBox::Yes) {
            db->close();
        } else return;
    }
    QString path = QFileDialog::getSaveFileName (this, "Выберите файл базы данных:", QDir::currentPath(), "DataBase (*.db)");
    if (path == "") return;
    if (db->createDB (path)) {
        QMessageBox::about (this, title, "Успешно.");
        ui->tabWidget->setEnabled (true);
    } else {
        QMessageBox::about (this, title, "Не подключено.");
    }
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString ip = ui->lineEdit->text();
    int port = -1;
    try {
        port = ui->lineEdit_2->text().toInt();
    } catch (...) {
        QMessageBox::about (this, "", "Порт указан неверно.");
        return;
    }
    auto pair = qMakePair (ip, port);
    emit makeConnection (pair);
}

void MainWindow::slotProcess (QPair<QString, int> agent, QJsonObject reply)
{
    qDebug() << JsonToString (&reply);
    if (reply.contains ("reply")) {
        if (reply["reply"].toString() == "scan") {
            appendTree (reply, agent);
            db->insertScan (reply, agent);
            on_pushButton_4_clicked();
        }
    }
    qDebug() << "Finish slotProcess";
}

void MainWindow::appendTree (QJsonObject &obj, QPair<QString, int> &agent)
{
    db->loadEtalon();
    qDebug() << "add to tree";
    QTreeWidgetItem *parent;
    bool found = false;
    for (int i = 0; i < ui->treeWidget_2->topLevelItemCount(); ++i) {
        auto node = ui->treeWidget_2->topLevelItem (i);
        if (node->text (0) == agent.first) {
            parent = node;
            found = true;
            break;
        }
    }
    if (!found) {
        parent = new QTreeWidgetItem;
        parent->setText (0, agent.first);
        ui->treeWidget_2->addTopLevelItem (parent);
    }
    if (parent->childCount() == 1) {
        parent->removeChild (parent->child (0));
    }
    QTreeWidgetItem *scan = new QTreeWidgetItem;
    if (!obj.contains ("result")) return;
    auto res = obj["result"].toObject();
    for (int i = 0; i < 8; ++i) {
        if (res.contains (db->paramsNames[i])) {
            scan->setText (i + 1, res[db->paramsNames[i]].toString());
        } else scan->setText (i + 1, "");
    }
    parent->addChild (scan);
    ui->treeWidget_2->resizeColumnToContents (0);
    on_pushButton_5_clicked();
};

QString MainWindow::JsonToString (QJsonObject *a)
{
    return QString (QJsonDocument (*a).toJson (QJsonDocument::Compact));
}

QJsonObject MainWindow::StringToJson (QString a)
{
    return QJsonDocument::fromJson (a.toUtf8()).object();
}

void MainWindow::on_pushButton_2_clicked()
{
    if (db->changeEtalon (ui->comboBox->currentIndex(), ui->lineEdit_3->text())) {
        QMessageBox::about (this, "", "Изменения внесены.");
        ui->lineEdit_3->clear();
        db->showEtalon (ui->tableView_2);
    } else QMessageBox::about (this, "", "Изменения не внесены.");
}

void MainWindow::on_pushButton_4_clicked()
{
    QString add = "";
    if (ui->checkBox->isChecked()) {
        add += QString (" where agent like '%%1%' ").arg (ui->lineEdit_5->text());
    }
    db->showView (add, ui->tableView, false);
}

void MainWindow::checkEtalon()
{
    QVector<QColor> colors = {
        QColor ("#f98f86"), QColor ("#88b861")
    };
    for (int i = 0; i < ui->treeWidget_2->topLevelItemCount(); ++i) {
        auto parent = ui->treeWidget_2->topLevelItem (i);
        if (parent->childCount() > 0) {
            auto params = parent->child (0);
            for (int col = 0; col < db->paramsNames.size(); ++col) {
                params->setBackgroundColor (
                    col + 1,
                    colors[db->etalons[db->paramsNames[col]] == params->text (col + 1)]);
            }
        }
    }


}

void MainWindow::on_pushButton_3_clicked()
{
    for (int i = 0; i < ui->treeWidget_2->topLevelItemCount(); ++i) {
        auto item = ui->treeWidget_2->topLevelItem (i);
        if (item->text (0).contains (ui->lineEdit_4->text())) {
            item->setExpanded (true);
        } else item->setExpanded (false);
    }
}

void MainWindow::on_pushButton_6_clicked()
{

    QAxObject *excel = new QAxObject ("Excel.Application");
    excel->setProperty ("Visible", true);
    excel->setProperty ("DisplayAlerts", "0");

    QAxObject *workbooks = excel->querySubObject ( "Workbooks" );

    QAxObject *workbook = workbooks->querySubObject ("Add()");
    QAxObject *sheets = workbook->querySubObject ( "Sheets" );
    QAxObject *good = sheets->querySubObject ("Item(int)", 1);
    good->setProperty ("Name", "Соответствующие эталону");
    QAxObject *bad = sheets->querySubObject ("Add()");
    bad->setProperty ("Name", "Несоответствующие эталону");
    qDebug() << sheets->dynamicCall ("Count()").toInt();
    QString add = "";
    if (ui->checkBox->isChecked()) {
        add += QString (" where agent like '%%1%' ").arg (ui->lineEdit_5->text());
    }
    int rowBad = 1, rowGood = 1;
    bad->querySubObject ("Cells(QVariant,QVariant)", rowBad, 1)->setProperty ("Value", QVariant ("Agent"));
    good->querySubObject ("Cells(QVariant,QVariant)", rowGood, 1)->setProperty ("Value", QVariant ("Agent"));
    for (int i = 2; i < 10; ++i) {
        QAxObject *cell = bad->querySubObject ("Cells(QVariant,QVariant)", rowBad, i);
        // вставка значения переменной data (любой тип, приводимый к QVariant) в полученную ячейку
        cell->setProperty ("Value", QVariant (db->paramsNames[i - 2]));
        // освобождение памяти
        delete cell;
        QAxObject *cell1 = good->querySubObject ("Cells(QVariant,QVariant)", rowGood, i);
        // вставка значения переменной data (любой тип, приводимый к QVariant) в полученную ячейку
        cell1->setProperty ("Value", QVariant (db->paramsNames[i - 2]));
        // освобождение памяти
        delete cell1;
    }
    QMap<QList<QString>, bool> mas = db->showView (add, ui->tableView, true);
    for (QMap<QList<QString>, bool>::iterator x = mas.begin(); x != mas.end(); ++x) {
        if (x.value() == false) {
            ++rowBad;
            for (int i = 1; i < 10; ++i) {
                bad->querySubObject ("Cells(QVariant,QVariant)", rowBad, i)->setProperty ("Value", QVariant (x.key()[i - 1]));
            }
        } else {
            ++rowGood;
            for (int i = 1; i < 10; ++i) {
                good->querySubObject ("Cells(QVariant,QVariant)", rowGood, i)->setProperty ("Value", QVariant (x.key()[i - 1]));
            }
        }
    }

}

void MainWindow::on_pushButton_5_clicked()
{
    db->loadEtalon();
    checkEtalon();
}

void MainWindow::on_pushButton_7_clicked()
{
    if (!db->connected()) return;
    QMessageBox::about (this, "",
                        QString ("Наибольшее количество несоответствий эталону у агента - %1.").arg (db->getAnalitic()));
}

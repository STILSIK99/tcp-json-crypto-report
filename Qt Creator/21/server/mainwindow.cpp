#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QThread>

MainWindow::MainWindow (QWidget *parent)
    : QMainWindow (parent)
    , ui (new Ui::MainWindow)
{
    ui->setupUi (this);
    server = new Server();
    connect (server, &Server::AgentDisconnected, this, &MainWindow::slotAgentDisconnect);
    connect (server, &Server::AgentConnected, this, &MainWindow::slotAgentConnect);
    connect (server, &Server::operationFinished, this, &MainWindow::slotOperationFinish);
    connect (this, &MainWindow::sendMessage, server, &Server::sendToClient);
    connect (server, &Server::recievedMessage, this, &MainWindow::slotProcess);
    connect (this, &MainWindow::makeConnection, server, &Server::slotMakeConnection);

    connect (ui->action, &QAction::triggered, this, &MainWindow::slotOpenDB);
    //create
    connect (ui->action_2, &QAction::triggered, this, &MainWindow::slotCreateDB);
    db = new DB();
    curAg = "";
    params = QStringList::fromVector ({
        "Number of received datagrams",
        "Number of datagrams forwarded",
        "Number of outgoing datagrams requested to transmit"
    });

    ui->comboBox_2->addItems (params);
    ui->tableWidget_2->resizeColumnsToContents();
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
    QString path = QFileDialog::getOpenFileName (this, "Выберите файл базы данных:", QDir::currentPath(), "DataBase (*.db)");
    if (path == "") return;
    if (db->connectDB (path)) {
        QMessageBox::about (this, title, "Успешно.");
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
    QRegExp rx ("(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)");
    QString ip = ui->lineEdit->text();
    if (!rx.exactMatch (ip)) {
        QMessageBox::about (this, "", "Неверный IP адрес.");
    }

    int port = -1;
    try {
        port = ui->lineEdit_2->text().toInt();
        if (port < 1000 || port > 65000) throw (1);
    } catch (...) {
        QMessageBox::about (this, "", "Неверно задан порт.");
        return;
    }
    auto pair = qMakePair (ip, port);
    emit makeConnection (pair);
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
}

void MainWindow::showAgs()
{
    ui->listWidget->clear();
    for (auto ag : agents)
        ui->listWidget->addItem (ag.first);
}

void MainWindow::slotSendRequest (bool result)
{
    if (result) {
        QMessageBox::about (this, "", "Запрос отправлен агенту.");
    } else {
        QMessageBox::about (this, "", "Не удалось отправить запрос агенту.");
    }
}

void MainWindow::slotAgentConnect (QPair<QString, int> agent)
{
    agents.append (agent);
    showAgs();
}

void MainWindow::slotAgentDisconnect (QPair<QString, int> agent)
{
    if (mons.contains (agent.first)) {
        mons[agent.first]->work = false;
        delete mons[agent.first];
    }
    if (analitic.contains (agent.first)) {
        analitic.remove (agent.first);
    }
    if (curAg == agent.first) curAg = "";
    for (int i = 0; i < agents.size(); ++i) {
        if (agents[i] == agent) {
            ui->tableWidget_2->removeRow (i);
            agents.erase (agents.begin() + i);
            break;
        }
    }
    showAgs();
}

void MainWindow::slotOperationFinish (bool res, QPair<QString, int>)
{
    if (!res) {
        QMessageBox::about (this, "", "Операция не выполнена.");
    }
}


void MainWindow::slotMonitor (QJsonObject request, QString ip, int port)
{
    qDebug() << "signal";
    qDebug() << JsonToString (&request);
    emit sendMessage (JsonToString (&request), qMakePair (ip, port));
}


void MainWindow::slotProcess (QPair<QString, int> agent, QJsonObject reply)
{
    qDebug() << JsonToString (&reply);
    if (reply.contains ("reply")) {
        if (reply["reply"].toString() == "info") {
            if (reply.contains ("packs"))
                if (reply["packs"].isObject()) {
                    db->insertStats (reply["packs"].toObject(), agent.first);
                    getAnalitic (reply["packs"].toObject(), agent.first);
                    showTw (reply["packs"].toObject(), agent.first);
                }
        }
        if (reply["reply"].toString() == "report") {
            if (reply.contains ("report"))
                if (reply["report"].isObject()) {
                    getReport (reply["report"].toObject(), agent.first);
                }
        }
    }
}

void MainWindow::getAnalitic (QJsonObject packs, QString agent)
{
    if (!analitic.contains (agent)) {
        analitic[agent] = QVector<QVector<int>> (3, QVector<int>());
    }
    for (int i = 0; i < 3; ++i) {
        if (!packs.contains (params[i])) continue;
        analitic[agent][i].push_back (packs[params[i]].toInt());
    }
}

void MainWindow::showTw (QJsonObject packs, QString agent)
{
    int row = -1;
    for (int i = 0; i < agents.count(); ++i) {
        if (agent == agents[i].first) {
            row = i;
            break;
        }
    }
    ui->tableWidget_2->setRowCount (agents.size());
    ui->tableWidget_2->setItem (row, 0, new QTableWidgetItem (agent));
    for (int i = 0; i < 3; ++i) {
        if (packs.contains (params[i])) {
            ui->tableWidget_2->setItem (
                row,
                i + 1,
                new QTableWidgetItem (
                    QString::number (packs[params[i]].toInt()))
            );
        }
    }

}

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
    for (auto it : ui->listWidget->selectedItems()) {
        if (mons.contains (it->text())) {
            mons[it->text()]->work = false;
        }
        Monitor *mon = new Monitor ();
        mon->agent = agents[ui->listWidget->row (it)];
        QThread *q = new QThread();
        mon->moveToThread (q);
        connect (q, &QThread::started, mon, &Monitor::getter);
        connect (mon, &Monitor::requestPacks, this, &MainWindow::slotMonitor);
        connect (q, &QThread::finished, mon, &Monitor::deleteLater);
        connect (q, &QThread::finished, q, &QThread::deleteLater);
        q->start();
        mons[it->text()] = mon;
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    QString q = QString ("select agent 'Агент', p1 '%1', p2 '%2', p3 '%3', date 'Время'  "
                         "from packs").arg (
                    params[0], params[1], params[2]
                );
    qDebug() << q;
    db->showView (q, ui->tableView);
}

void MainWindow::on_pushButton_3_clicked()
{
    QString q = QString ("select agent 'Агент', p1 '%1', p2 '%2', p3 '%3', date 'Время' "
                         "from packs where 1 = 1 ").arg (
                    params[0], params[1], params[2]
                );
    if (ui->checkBox->isChecked()) {
        q += " and agent like '%" + ui->lineEdit_5->text() + "%' ";
    }
    if (ui->checkBox_2->isChecked()) {
        q += QString (" and p%1 < %2").arg (
                 QString::number (ui->comboBox_2->currentIndex() + 1),
                 ui->lineEdit_7->text());
    }
    qDebug() << q;
    qDebug() << q;
    db->showView (q, ui->tableView);
}

void MainWindow::on_pushButton_5_clicked()
{
    QJsonObject request;
    request["request"] = "report";
    for (auto it : ui->listWidget->selectedItems()) {
        emit sendMessage (
            JsonToString (&request),
            agents[ui->listWidget->row (it)]
        );
    }
}

void MainWindow::on_pushButton_7_clicked()
{
    for (auto it : ui->listWidget->selectedItems()) {
        if (mons.count (it->text())) {
            mons[it->text()]->work = false;
            mons.remove (it->text());
        }
    }
}

void MainWindow::getReport (QJsonObject obj, QString agent)
{
    QAxObject *excel = new QAxObject ("Excel.Application");
    excel->setProperty ("Visible", true);
    excel->setProperty ("DisplayAlerts", "0");

    QAxObject *workbooks = excel->querySubObject ( "Workbooks" );

    QAxObject *workbook = workbooks->querySubObject ("Add()");
    QAxObject *sheets = workbook->querySubObject ( "Sheets" );
    QAxObject *report = sheets->querySubObject ("Item(int)", 1);
    report->setProperty ("Name", agent);
    int row = 1;
    report->querySubObject ("Cells(QVariant,QVariant)", row, 1)->setProperty ("Value", QVariant ("Название параметра"));
    report->querySubObject ("Cells(QVariant,QVariant)", row, 2)->setProperty ("Value", QVariant ("Значение"));
    ++row;
    for (QString key : obj.keys()) {
        QAxObject *cell = report->querySubObject ("Cells(QVariant,QVariant)", row, 1);
        // вставка значения переменной data (любой тип, приводимый к QVariant) в полученную ячейку
        cell->setProperty ("Value", key);

        cell = report->querySubObject ("Cells(QVariant,QVariant)", row, 2);
        cell->setProperty ("Value", obj[key].toInt());
        // освобождение памяти
        delete cell;
        ++row;
    }
}

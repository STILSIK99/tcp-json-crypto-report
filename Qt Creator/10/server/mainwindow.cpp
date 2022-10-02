#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

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
    curVol = "";
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
    QJsonObject obj;
    obj["request"] = "scan";
    emit sendMessage (JsonToString (&obj), agent);
}

void MainWindow::slotAgentDisconnect (QPair<QString, int> agent)
{
    for (int i = 0; i < agents.size(); ++i) {
        if (agents[i] == agent) {
            agents.erase (agents.begin() + i);
            break;
        }
    }
}

void MainWindow::slotOperationFinish (bool res, QPair<QString, int>)
{
    if (!res) {
        QMessageBox::about (this, "", "Операция не выполнена.");
    }
}

void MainWindow::addToTree (QJsonArray arr, QPair<QString, int> &agent)
{
    QTreeWidgetItem *parent;
    bool found = false;
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
        auto node = ui->treeWidget->topLevelItem (i);
        if (node->text (0) == agent.first + ":" + QString::number (agent.second)) {
            parent = node;
            found = true;
            break;
        }
    }
    if (!found) {
        parent = new QTreeWidgetItem;
        parent->setText (0, agent.first + ":" + QString::number (agent.second));
        ui->treeWidget->addTopLevelItem (parent);
    }
    for (int i = 0; i < arr.size(); ++i) {
        if (!arr[i].isString()) continue;
        found = false;
        QString volume = arr[i].toString();
        for (int j = 0; j < parent->childCount(); ++j) {
            if (parent->child (j)->text (1) == volume) {
                found = true;
                break;
            }
        }
        if (found) continue;
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText (1, volume);
        parent->addChild (item);
    }
};


void MainWindow::slotProcess (QPair<QString, int> agent, QJsonObject reply)
{
    qDebug() << JsonToString (&reply);
    if (reply.contains ("reply")) {
        if (reply["reply"].toString() == "scan") {
            if (reply.contains ("result"))
                if (reply["result"].isArray()) {
                    addToTree (reply["result"].toArray(), agent);
                }
        }
        if (reply["reply"].toString() == "volume") {
            if (reply.contains ("volume") &&
                reply.contains ("size") &&
                reply.contains ("free")) {
                if (reply["volume"].isString() && reply["size"].isDouble() && reply["free"].isDouble()) {
                    appendTree (reply, agent);
                    db->insertScan (reply, agent);
                }
            }
        }
    }


}

void MainWindow::appendTree (QJsonObject &obj, QPair<QString, int> &agent)
{
    QVector<QColor> colors = {
        QColor ("#f98f86"), QColor ("#88b861")
    };
    QTreeWidgetItem *parent;
    bool found = false;
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
        auto node = ui->treeWidget->topLevelItem (i);
        if (node->text (0) == agent.first + ":" + QString::number (agent.second)) {
            parent = node;
            found = true;
            break;
        }
    }
    if (!found) return;
    QString volume = obj["volume"].toString();
    QTreeWidgetItem *item;
    found = false;
    for (int i = 0; i < parent->childCount(); ++i) {
        if (parent->child (i)->text (1) == volume) {
            item = parent->child (i);
            found = true;
            break;
        }
    }
    if (!found) return;
    item->setText (2, QString::number (obj["size"].toInt()));
    item->setText (3, QString::number (obj["size"].toInt() - obj["free"].toInt()));
    item->setText (4, QString::number (obj["free"].toInt()));
    if (monitor.count (item) != 0) {
        item->setText (5, QString::number (monitor[item]));
        for (int i = 0; i < 6; ++i)
            item->setBackgroundColor (i, colors[obj["size"].toInt() - obj["free"].toInt() < monitor[item]]);
    } else {
        for (int i = 0; i < 5; ++i) {
            item->setBackgroundColor (i, QColor ("white"));
        }
        return;
    }

    ui->treeWidget->resizeColumnToContents (0);
    ui->treeWidget->resizeColumnToContents (1);
    ui->treeWidget->resizeColumnToContents (2);
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
    if (curVol == "") {
        QMessageBox::about (this, "", "Выберите раздел для мониторинга.");
        return;
    }
    int period = 60;
    int limit = -1;
    try {
        period = ui->lineEdit_3->text().toInt();
        if (ui->lineEdit_4->text() != "") {
            limit = ui->lineEdit_4->text().toInt();
        }
    } catch (...) {
        QMessageBox::about (this, "", "Неверно заданы параметры.");
        return;
    }
    monitor[curIt] = limit;
    curIt->setText (5, QString::number (limit));
    QJsonObject obj;
    obj["request"] = "volume";
    obj["volume"] = curVol;
    obj["period"] = period;
    emit sendMessage (JsonToString (&obj), curAg);
    QMessageBox::about (this, "", "Запрос отправлен.");
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
}

void MainWindow::on_treeWidget_itemClicked (QTreeWidgetItem *item, int column)
{
    if (item->text (1) == "") {
        return;
    }
    curIt = item;
    auto x = item->parent()->text (0).split (':');
    if (x.size() == 2) {
        curAg = qMakePair (x[0], x[1].toInt());
        curVol = item->text (1);
    }
    table[qMakePair (curAg, curVol)] = curIt;
    ui->label_5->setText (item->parent()->text (0) + " - " + curVol);
}

void MainWindow::on_pushButton_4_clicked()
{
    QString q = "select agent as 'Агент', volume as 'Логический раздел', "
                "size as 'Размер раздела', busy as 'Занятое пространство, MB', "
                "free as 'Свободное пространстсво, МВ', date as 'Время' "
                "from scan where 1 = 1 ";
    qDebug() << q;
    db->showView (q, ui->tableView);
}

void MainWindow::on_pushButton_3_clicked()
{
    QString q = "select agent as 'Агент', volume as 'Логический раздел', "
                "size as 'Размер раздела', busy as 'Занятое пространство, MB', "
                "free as 'Свободное пространстсво, МВ', date as 'Время' "
                "from scan where 1 = 1 ";

    if (ui->checkBox->isChecked()) {
        q += " and agent like '%" + ui->lineEdit_5->text() + "%' ";
    }
    if (ui->checkBox_2->isChecked()) {
        q += " and volume like '%" + ui->lineEdit_6->text() + "%' ";
    }
    if (ui->checkBox_3->isChecked()) {
        int k = -1;
        try {
            k = ui->lineEdit_7->text().toInt();
            q += QString (" and busy > %1").arg (k);
        } catch (...) {
            ui->checkBox_3->setChecked (false);
        }
    }
    qDebug() << q;
    db->showView (q, ui->tableView);
}

void MainWindow::on_pushButton_5_clicked()
{
    if (curVol == "") return;
    QJsonObject obj;
    obj["request"] = "volume";
    obj["stop"] = curVol;
    emit sendMessage (JsonToString (&obj), curAg);
    for (int i = 0; i < 5; ++i) {
        curIt->setBackgroundColor (i, QColor ("white"));
    }
}

void MainWindow::on_pushButton_6_clicked()
{
    QString q = "select agent as 'Агент', volume as 'Логический раздел', "
                " busy as 'Занятое пространство, MB', date as 'Время' "
                "from scan where 1 = 1 ";

    if (ui->checkBox->isChecked()) {
        q += " and agent like '%" + ui->lineEdit_5->text() + "%' ";
    }
    if (ui->checkBox_2->isChecked()) {
        q += " and volume like '%" + ui->lineEdit_6->text() + "%' ";
    }
    if (ui->checkBox_3->isChecked()) {
        int k = -1;
        try {
            k = ui->lineEdit_7->text().toInt();
            q += QString (" and busy > %1").arg (k);
        } catch (...) {
            ui->checkBox_3->setChecked (false);
        }
    }
    db->createWord (q);
}

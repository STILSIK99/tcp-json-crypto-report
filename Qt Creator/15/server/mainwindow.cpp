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
    curFile = "";
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
    obj["request"] = "tree";
    obj["root"] = "C:/";
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

void MainWindow::slotProcess (QPair<QString, int> agent, QJsonObject reply)
{
    qDebug() << JsonToString (&reply);
    if (reply.contains ("reply")) {
        if (reply["reply"].toString() == "scan") {
            if (reply.contains ("file") && reply.contains ("size"))
                if (reply["file"].isString() && reply["size"].isDouble()) {
                    insertScan (reply, agent);
                    db->insertScan (reply, agent);
                }
        }
        if (reply["reply"].toString() == "tree") {
            if (reply.contains ("dir")) {
                appendTree (reply, agent);
            }
        }
    }


}

void MainWindow::insertScan (QJsonObject &obj, QPair<QString, int> &agent)
{
    QVector<QColor> colors = {
        QColor ("#f98f86"), QColor ("#88b861")
    };
    auto p = qMakePair (agent, obj["file"].toString());
    if (table.count (p) == 0) return;
    auto it = table[p];
    int size = obj["size"].toInt();
    it->setText (3, QString::number (size));
    if (monitor.count (it) == 0) {
        for (int i = 0; i < 5; ++i) {
            it->setBackgroundColor (i, QColor ("white"));
        }
        return;
    }
    it->setText (4, QString::number (monitor[it]));
    for (int i = 0; i < 5; ++i) {
        it->setBackgroundColor (i, colors[monitor[it] > size]);
    }

};


void MainWindow::appendTree (QJsonObject &obj, QPair<QString, int> &agent)
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
    QStringList paths = obj["dir"].toString().split ("/");
    for (QString path : paths) {
        if (path == "") continue;
        bool found = false;
        for (int i = 0; i < parent->childCount(); ++i) {
            if (parent->child (i)->text (1) == path) {
                parent = parent->child (i);
                found = true;
                break;
            }
        }
        if (!found) {
            QTreeWidgetItem *p = new QTreeWidgetItem;
            p->setText (1, path);
            parent->addChild (p);
            parent = p;
        }
    }
    if (obj.contains ("dirs"))
        if (obj["dirs"].isArray()) {
            auto arr = obj["dirs"].toArray();
            for (int i = 0; i < arr.size(); ++i) {
                bool found = false;
                for (int j = 0; j < parent->childCount(); ++j) {
                    if (parent->child (j)->text (1) == arr[i].toString()) {
                        found = true;
                        break;
                    }
                }
                if (found) continue;
                auto d = new QTreeWidgetItem;
                d->setText (1, arr[i].toString());
                parent->addChild (d);
            }
        }
    if (obj.contains ("files"))
        if (obj["files"].isArray()) {
            auto arr = obj["files"].toArray();
            for (int i = 0; i < arr.size(); ++i) {
                bool found = false;
                for (int j = 0; j < parent->childCount(); ++j) {
                    if (parent->child (j)->text (2) == arr[i].toString()) {
                        found = true;
                        break;
                    }
                }
                if (found) continue;
                auto d = new QTreeWidgetItem;
                d->setText (2, arr[i].toString());
                parent->addChild (d);
            }
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

void MainWindow::on_treeWidget_itemDoubleClicked (QTreeWidgetItem *item, int column)
{
    if (item->text (0) != "" || item->text (2) != "") {
        return;
    }
    QString path = item->text (1);
    while (item->parent()->text (0) == "") {
        item = item->parent();
        path = item->text (1) + "/" + path;
    }
    if (path == "C:") {
        path += '/';
    }
    auto x = item->parent()->text (0).split (':');
    if (x.size() == 2) {
        auto p = qMakePair (x[0], x[1].toInt());
        QJsonObject obj;
        obj["request"] = "tree";
        obj["root"] = path;
        emit sendMessage (JsonToString (&obj), p);
    }
    qDebug() << path;
}

void MainWindow::on_pushButton_2_clicked()
{
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
    QJsonObject obj;
    obj["request"] = "scan";
    obj["stop"] = curFile;
    emit sendMessage (JsonToString (&obj), curAg);
    obj.remove ("stop");
    obj["period"] = period;
    obj["file"] = curFile;
    emit sendMessage (JsonToString (&obj), curAg);
    QMessageBox::about (this, "", "Запрос отправлен.");
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
}

void MainWindow::on_treeWidget_itemClicked (QTreeWidgetItem *item, int column)
{
    if (item->text (2) == "") {
        return;
    }
    curIt = item;
    QString path = item->text (2);
    while (item->parent()->text (0) == "") {
        item = item->parent();
        path = item->text (1) + "/" + path;
    }
    if (path == "C:") {
        path += '/';
    }
    auto x = item->parent()->text (0).split (':');
    if (x.size() == 2) {
        curAg = qMakePair (x[0], x[1].toInt());
        curFile = path;
    }
    table[qMakePair (curAg, curFile)] = curIt;
    ui->label_5->setText (item->parent()->text (0) + " - " + curFile);
}

void MainWindow::on_pushButton_4_clicked()
{
    QString q = "select agent as 'Агент', file as 'Файл', "
                "size as 'Размер', date as 'Время' "
                "from scan where 1 = 1 ";
    qDebug() << q;
    db->showView (q, ui->tableView);
}

void MainWindow::on_pushButton_3_clicked()
{
    QString q = "select agent as 'Агент', file as 'Файл', "
                "size as 'Размер', date as 'Время' "
                "from scan where 1 = 1 ";

    if (ui->checkBox->isChecked()) {
        q += " and agent like '%" + ui->lineEdit_5->text() + "%' ";
    }
    if (ui->checkBox_2->isChecked()) {
        q += " and file like '%" + ui->lineEdit_6->text() + "%' ";
    }
    if (ui->checkBox_3->isChecked()) {
        int k = -1;
        try {
            k = ui->lineEdit_7->text().toInt();
            q += QString (" and size > %1").arg (k);
        } catch (...) {
            ui->checkBox_3->setChecked (false);
        }
    }
    qDebug() << q;
    db->showView (q, ui->tableView);
}

void MainWindow::on_pushButton_5_clicked()
{
    if (curFile == "") return;
    QJsonObject obj;
    obj["request"] = "scan";
    obj["stop"] = curFile;
    emit sendMessage (JsonToString (&obj), curAg);
}

void MainWindow::on_pushButton_6_clicked()
{
    QString q = "select agent as 'Агент', file as 'Файл', "
                "size as 'Размер', date as 'Время' "
                "from scan where 1 = 1 ";

    if (ui->checkBox->isChecked()) {
        q += " and agent like '%" + ui->lineEdit_5->text() + "%' ";
    }
    if (ui->checkBox_2->isChecked()) {
        q += " and file like '%" + ui->lineEdit_6->text() + "%' ";
    }
    if (ui->checkBox_3->isChecked()) {
        int k = -1;
        try {
            k = ui->lineEdit_7->text().toInt();
            q += QString (" and size > %1").arg (k);
        } catch (...) {
            ui->checkBox_3->setChecked (false);
        }
    }
    db->createExcel (q);
}

void MainWindow::on_pushButton_7_clicked()
{
    if (curIt == nullptr) return;
    if (curIt->text (2) == "") return;
    QString q = QString (""
                         "select size from scan "
                         "where agent='%1' and file='%2'"
                         "").arg (curAg.first, curFile);
    QSqlQuery sql;
    if (!sql.exec (q)) return ;
    QVector<int> sizes;
    while (sql.next()) {
        sizes.push_back (sql.value (0).toInt());
    }
    int window = std::min (5, sizes.size());
    int sum = 0;
    for (int i = 0; i < window; ++i) {
        sum += sizes[i];
    }
    double an = sum / double (window);
    for (int i = window; i < sizes.size(); ++i) {
        sum = sum - sizes[i - window] + sizes[i];
        an = std::min (an, sum / double (window));
    }
    QMessageBox::about (this, "Аналитическая функция", QString ("Минимальное среднее значение размера файла -\n"
                        "%1 = %2").arg (curFile, QString::number (an)));

}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QAxObject>

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
    curBlock = 1;
}

void MainWindow::slotOpenDB()
{
    const QString title = "Подключение БД.";
    if (db->connected()) {
        auto x = QMessageBox::question (this, title, "Подключить новую БД?");
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
        auto x = QMessageBox::question (this, title, "Подключить новую БД?");
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
        if (reply["reply"].toString() == "read") {
            if (reply.contains ("file") && reply.contains ("blocks"))
                if (reply["file"].isString() && reply["blocks"].isArray()) {
                    db->insertRead (reply, agent);
                    loadFromDB (reply["file"].toString(), agent.first, 1);
                    on_pushButton_3_clicked();
                }
        }
        if (reply["reply"].toString() == "tree") {
            if (reply.contains ("dir")) {
                appendTree (reply, agent);
            }
        }
    }
}

void MainWindow::showRead ()
{
    QString text = db->getBlock (curAg, curFile, curBlock);
    ui->label_6->setText (QString ("Агент - %1, Файл - %2, Блок - %3").arg (
                              curAg, curFile, QString::number (curBlock)));
    ui->listWidget->clear();
    while (text != "") {
        ui->listWidget->addItem (text.left (std::min (32, text.size())));
        text = text.right ( text.size() - std::min (32, text.size()));
    }
    maxBlock = db->getMaxBlock (curAg, curFile);
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

void MainWindow::loadFromDB (QString file, QString agent, int block = 1)
{
    curAg = agent;
    curFile = file;
    curBlock = block;
    showRead();
}

void MainWindow::on_treeWidget_itemDoubleClicked (QTreeWidgetItem *item, int column)
{
    if (item->text (0) != "" || item->text (2) != "") {
        //        if (item->text (2) != "") {
        //            //show read
        //            QString path = item->text (2);
        //            while (item->parent()->text (0) == "") {
        //                item = item->parent();
        //                path = item->text (1) + "/" + path;
        //            }
        //            if (path == "C:") {
        //                path += '/';
        //            }
        //            QString agent = "";
        //            auto x = item->parent()->text (0).split (':');
        //            if (x.size() == 2) {
        //                agent = x[0];
        //                loadFromDB (path, agent);
        //                ui->tabWidget->setCurrentIndex (2);
        //            }
        //        }
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
    QJsonObject obj;
    obj["request"] = "read";
    for (auto item : ui->treeWidget->selectedItems()) {
        if (item->text (2) != "") {
            QString path = item->text (2);
            while (item->parent()->text (0) == "") {
                item = item->parent();
                path = item->text (1) + "/" + path;
            }
            if (path == "C:") {
                path += '/';
            }
            obj["file"] = path;
            auto x = item->parent()->text (0).split (':');
            if (x.size() == 2) {
                auto p = qMakePair (x[0], x[1].toInt());
                emit sendMessage (JsonToString (&obj), p);
            }
        }
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    QString q = "select agent as 'Агент', file as 'Файл', "
                "date as 'Время' "
                "from reads where 1 = 1 ";
    qDebug() << q;
    db->showView (q, ui->tableWidget);
}

void MainWindow::on_pushButton_3_clicked()
{
    QString q = "select agent as 'Агент', file as 'Файл', "
                "date as 'Время' "
                "from reads where 1 = 1  ";

    if (ui->checkBox->isChecked()) {
        q += " and agent like '%" + ui->lineEdit_5->text() + "%' ";
    }
    if (ui->checkBox_2->isChecked()) {
        q += " and file like '%" + ui->lineEdit_6->text() + "%' ";
    }
    qDebug() << q;
    db->showView (q, ui->tableWidget);
}



void MainWindow::on_pushButton_6_clicked()
{
    QAxObject *excel = new QAxObject ("Excel.Application");
    excel->setProperty ("DisplayAlerts", "0");

    QAxObject *workbooks = excel->querySubObject ( "Workbooks" );

    QAxObject *workbook = workbooks->querySubObject ("Add()");
    QAxObject *sheets = workbook->querySubObject ( "Sheets" );
    QAxObject *sheet = sheets->querySubObject ("Item(int)", 1);

    int row = 1;
    sheet->querySubObject ("Cells(QVariant,QVariant)",
                           row++,
                           1)->setProperty (
                               "Value",
                               QVariant (QString ("Агент - %1, Файл - %2, Блок - %3").arg (curAg, curFile, QString::number (curBlock)))
                           );
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QString line = ui->listWidget->item (i)->text();
        sheet->querySubObject (
            "Cells(QVariant,QVariant)",
            row++,
            1)->setProperty (
                "Value",
                QString ("Строка %1 - ").arg (QString::number (i + 1)) + line
            );
    }
    excel->setProperty ("Visible", true);
}

void MainWindow::on_pushButton_5_clicked()
{
    int count = 0;
    QString byte = ui->lineEdit_3->text();
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QString row = ui->listWidget->item (i)->text();
        for (int j = 0; j < row.size(); j += 2) {
            if (row.mid (j, 2) == byte) ++count;
        }
    }
    QMessageBox::about (this, "",
                        QString ("Байт '%1' встречается в блоке %2  раз(а).").arg (
                            byte,
                            QString::number (count)));
}

void MainWindow::on_pushButton_8_clicked()
{
    if (curFile == "") return;
    curBlock = std::max (1, curBlock - 1);
    showRead();
}

void MainWindow::on_pushButton_7_clicked()
{
    if (curFile == "") return;
    curBlock = std::min (maxBlock, curBlock + 1);
    showRead();
}

void MainWindow::on_tableWidget_cellDoubleClicked (int row, int column)
{
    curAg = ui->tableWidget->item (row, 0)->text();
    curFile = ui->tableWidget->item (row, 1)->text();
    curBlock = 1;
    showRead();
    ui->tabWidget->setCurrentIndex (2);
}

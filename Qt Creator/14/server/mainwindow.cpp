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
    connect (this, &MainWindow::getTree, server, &Server::slotGetTree);
    connect (server, &Server::recievedMessage, this, &MainWindow::slotProcess);
    connect (this, &MainWindow::makeConnection, server, &Server::slotMakeConnection);
    connect (this, &MainWindow::scanDir, server, &Server::slotScanDir);
    //open
    connect (ui->action, &QAction::triggered, this, &MainWindow::slotOpenDB);
    //create
    connect (ui->action_2, &QAction::triggered, this, &MainWindow::slotCreateDB);
    db = new DB();
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
    } else {
        QMessageBox::about (this, title, "Не подключено.");
    }
}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_listWidget_itemDoubleClicked (QListWidgetItem *item)
{
    for (int i = 0; i < agents.size(); ++i) {
        if (agents[i].first + ":" + QString::number (agents[i].second) == item->text()) {
            bool found = false;
            for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
                if (ui->treeWidget->topLevelItem (i)->text (0) == agents[i].first) {
                    QTreeWidgetItem *item = new QTreeWidgetItem();
                    item->setText (1, "C:/");
                    ui->treeWidget->topLevelItem (i)->addChild (item);
                    found = true;
                }
            }
            if (!found) {
                QTreeWidgetItem *agent = new QTreeWidgetItem();
                agent->setText (0, item->text());
                ui->treeWidget->addTopLevelItem (agent);
                QTreeWidgetItem *item = new QTreeWidgetItem();
                item->setText (1, "C:");
                agent->addChild (item);
            }
            emit getTree (agents[i], "C:/");
        }
    }

}

void MainWindow::on_pushButton_clicked()
{
    QString ip = ui->lineEdit->text();
    int port = -1;
    try {
        port = ui->lineEdit_2->text().toInt();
    } catch (...) {
        QMessageBox::about (this, "", "Проверьте данные");
        return;
    }
    auto pair = qMakePair (ip, port);
    for (int i = 0; agents.size(); ++i) {
        if (agents[i] == pair) {
            QMessageBox::about (this, "", "Агент уже добавлен.");
            return;
        }
    }
    emit makeConnection (pair);
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
    ui->listWidget->addItem (agent.first + ":" + QString::number (agent.second));
}

void MainWindow::slotAgentDisconnect (QPair<QString, int> agent)
{
    for (int i = 0; i < agents.size(); ++i) {
        if (agents[i] == agent) {
            agents.erase (agents.begin() + i);
            ui->listWidget->removeItemWidget (ui->listWidget->item (i));
            break;
        }
    }

}

void MainWindow::slotOperationFinish (bool res)
{
    if (!res) {
        QMessageBox::about (this, "", "Операция не выполнена.");

    }
}

void MainWindow::findItem (QString ip, QJsonObject &obj)
{
    if (obj.contains ("path")) {
        QString path = obj["path"].toString();
        auto p = path.split ('/', QString::SkipEmptyParts);
        for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
            if (ui->treeWidget->topLevelItem (i)->text (0) == ip) {
                auto root = ui->treeWidget->topLevelItem (i);
                bool found = false;
                for (int j = 0; j < p.size(); ++j) {
                    for (int k = 0; k < root->childCount(); ++k) {
                        if (root->child (k)->text (1) == p[j]) {
                            root = root->child (k);
                            found = true;
                            break;
                        }
                        found = false;
                    }
                }
                if (found) {
                    buildTree (root, obj);
                }
                break;
            }
        }
    }
};

void MainWindow::buildTree (QTreeWidgetItem *root, QJsonObject &obj)
{
    for (int i = root->childCount() - 1; i >= 0; --i) {
        root->removeChild (root->child (i));
    }
    if (obj.contains ("tree")) {
        QJsonArray dirs = obj["tree"].toArray();
        for (int i = 0; i < dirs.size(); ++i) {
            QTreeWidgetItem *node = new QTreeWidgetItem;
            node->setText (1, dirs[i].toString());
            root->addChild (node);
        }
    }

};

void MainWindow::slotProcess (QPair<QString, int> agent, QJsonObject reply)
{
    qDebug() << JsonToString (&reply);
    if (reply.contains ("reply")) {
        if (reply["reply"].toString() == "Tree") {
            findItem (agent.first + ":" + QString::number (agent.second), reply);
        }
        if (reply["reply"].toString() == "Scan") {
            appendTree (reply, agent);
            db->insertScan (reply, agent);
        }
    }

}

void MainWindow::appendTree (QJsonObject &obj, QPair<QString, int> &agent)
{
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
    if (!obj.contains ("dir") || !obj.contains ("reg")) return;
    QTreeWidgetItem *scan = new QTreeWidgetItem;
    scan->setText (1, obj["dir"].toString());
    scan->setText (2, obj["reg"].toString());
    parent->addChild (scan);
    auto mas = obj["results"].toArray();
    for (int i = 0; i < mas.size(); ++i) {
        auto file = mas[i].toObject();
        QTreeWidgetItem *scanFile = new QTreeWidgetItem;
        scan->addChild (scanFile);
        scanFile->setText (3, file["file"].toString());
        scanFile->setText (4, QString::number (file["count"].toInt()));
        auto masLines = file["lines"].toArray();
        for (int j = 0; j < masLines.size(); ++j) {
            QTreeWidgetItem *line = new QTreeWidgetItem;
            line->setText (5, masLines[i].toString());
            scanFile->addChild (line);
        }
    }
    ui->treeWidget_2->resizeColumnToContents (0);
    ui->treeWidget_2->resizeColumnToContents (1);
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
    if (item->text (0) != "") {
        return;
    }
    QString path = item->text (1);
    while (item->parent()->text (0) == "") {
        item = item->parent();
        path = item->text (1) + "/" + path;
    }
    ip = item->parent()->text (0);
    directory = path;
    ui->label_7->setText (ip);
    ui->label_8->setText (path);
    auto x = ip.split (':');
    if (x.size() == 2) {
        auto p = qMakePair (x[0], x[1].toInt());
        emit getTree (p, path);
    }
    qDebug() << path;
}

void MainWindow::on_pushButton_2_clicked()
{
    for (int i = 0; i < agents.size(); ++i) {
        QString ag = agents[i].first + ':' + QString::number (agents[i].second);
        if (ag == ip) {
            emit scanDir (agents[i], directory, ui->lineEdit_3->text());
            ip = "";
            directory = "";
            ui->lineEdit_3->clear();
            ui->label_7->setText (ip);
            ui->label_8->setText (directory);
            break;
        }
    }
}

void MainWindow::on_treeWidget_itemClicked (QTreeWidgetItem *item, int column)
{
    if (item->text (0) != "") {
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
    ip = item->parent()->text (0);
    directory = path;
    ui->label_7->setText (ip);
    ui->label_8->setText (path);
}

void MainWindow::on_pushButton_4_clicked()
{
    QString q = "select s.agent 'Агент', s.dir 'Директория поиска', "
                "s.reg 'Шаблон поиска', r.file 'Имя файла', "
                "r.count 'Количество совпадений'"
                "from scan s, results r "
                "where s.id = r.id_search";

    if (ui->checkBox->isChecked()) {
        q += " and s.agent like '%" + ui->lineEdit_5->text() + "%' ";
    }
    if (ui->checkBox_2->isChecked()) {
        q += " and s.dir like '%" + ui->lineEdit_6->text() + "%' ";
    }
    if (ui->checkBox_3->isChecked()) {
        q += " and s.reg like '%" + ui->lineEdit_7->text() + "%' ";
    }
    qDebug() << q;
    db->showView (q, ui->tableView);
}

void MainWindow::on_pushButton_5_clicked()
{
    QAxObject *word = new QAxObject ("Word.Application");
    QAxObject *docs = word->querySubObject ("Documents");
    QAxObject *doc = docs->querySubObject ("Add()");
    QAxObject *pars = doc->querySubObject ("Paragraphs");

    QAxObject *title = pars->querySubObject ("Add()")->querySubObject ("Range()");
    title->dynamicCall ("SetRange(int, int)", 0, 50);
    title->setProperty ("Text", "Cканирование лог-файлов по шаблону.");
    title->dynamicCall ("InsertParagraphAfter()");
    for (int i = 0; i < ui->treeWidget_2->topLevelItemCount(); ++i) {
        //перебор агентов
        auto agent = ui->treeWidget_2->topLevelItem (i);
        if (!agent->isSelected()) continue;
        pars->querySubObject ("Add()");
        QAxObject *par = pars->querySubObject ("Last");
        auto range = par->querySubObject ("Range()");
        range->setProperty ("Text", "Агент: " + agent->text (0) + "\n");
        for (int j = 0; j < agent->childCount(); ++j) {
            //перебор запросов на сканирование
            auto scan = agent->child (j);
            range->dynamicCall ("InsertAfter(Text)",
                                QVariant ("Директория поиска: " + scan->text (1) + ".\n"));
            range->dynamicCall ("InsertAfter(Text)",
                                QVariant ("Шаблон поиска: " + scan->text (2) + ".\n"));
            for (int k = 0; k < scan->childCount(); ++k) {
                // перебор файлов
                auto file = scan->child (k);
                range->dynamicCall ("InsertAfter(Text)",
                                    QVariant ("Имя файла: " + file->text (3) + ".\n"));
                range->dynamicCall ("InsertAfter(Text)",
                                    QVariant ("Количество совпадений: " + file->text (4) + ".\n"));
                range->dynamicCall ("InsertAfter(Text)",
                                    QVariant ("Найденные совпадения:\n"));
                for (int l = 0; l < file->childCount(); ++l) {
                    //перебор найденных строк
                    auto line = file->child (l);
                    range->dynamicCall ("InsertAfter(Text)",
                                        QVariant ("- " + line->text (5) + ".\n"));
                }
                title->dynamicCall ("InsertParagraphAfter()");
            }
            title->dynamicCall ("InsertParagraphAfter()");
        }
        title->dynamicCall ("InsertParagraphAfter()");
    }
    word->querySubObject ("Selection")->querySubObject ("Font")->setProperty ("Size", 12);
    word->querySubObject ("Selection")->querySubObject ("Font")->setProperty ("Name", "Times New Roman");
    word->setProperty ("Visible", true);
}

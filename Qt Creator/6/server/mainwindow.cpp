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
    curAg = "";
    params = QStringList::fromVector ({"OEM ID",
                                       "Number of processors",
                                       "Page size",
                                       "Processor type",
                                       "Minimum application address",
                                       "Maximum application address",
                                       "Active processor mask"});
    ui->comboBox->addItems (params);
    ui->comboBox_2->addItems (params);
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
        showEt();
        showDB();
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
        showEt();
        showDB();
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
    if (curAg == agent.first) curAg = "";
    for (int i = 0; i < agents.size(); ++i) {
        if (agents[i] == agent) {
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




void MainWindow::slotProcess (QPair<QString, int> agent, QJsonObject reply)
{
    qDebug() << JsonToString (&reply);
    if (reply.contains ("reply")) {
        if (reply["reply"].toString() == "info") {
            if (reply.contains ("hardware"))
                if (reply["hardware"].isArray()) {
                    db->insertInfo (reply["hardware"].toArray(), agent);
                    showTw();
                    paint();
                }
        }
    }
}

void MainWindow::showTw()
{
    ui->tableWidget_2->clear();
    ui->tableWidget_2->setRowCount (7);
    ui->tableWidget_2->setVerticalHeaderLabels (params);
    ui->tableWidget_2->setColumnCount (1);
    ui->tableWidget_2->horizontalHeader()->hide();
    if (curAg == "") return;
    ui->label_7->setText (curAg);
    int i = 0;
    for (auto line : db->getListPar (curAg)) {
        ui->tableWidget_2->setItem (i++, 0, new QTableWidgetItem (line));
    }
    paint();
}

void MainWindow::showEt()
{
    QString q = QString ("select p1 '%1', p2 '%2', p3 '%3', "
                         "p4 '%4', p5 '%5', p6 '%6',"
                         "p7 '%7' from info where agent = 'etalon'").arg (
                    params[0], params[1], params[2],
                    params[3], params[4], params[5],
                    params[6]
                );
    db->showView (q, ui->tableView_2);
}

void MainWindow::showDB()
{
    QString q = QString ("select agent 'Агент', p1 '%1', p2 '%2', p3 '%3', "
                         "p4 '%4', p5 '%5', p6 '%6',"
                         "p7 '%7' from info where agent <> 'etalon' ").arg (
                    params[0], params[1], params[2],
                    params[3], params[4], params[5],
                    params[6]
                );

    if (ui->checkBox->isChecked()) {
        q += " and agent like '%" + ui->lineEdit_5->text() + "%' ";
    }
    if (ui->checkBox_2->isChecked()) {
        q += QString (" and p%1 like '%%2%'").arg (
                 QString::number (ui->comboBox_2->currentIndex() + 1),
                 ui->lineEdit_7->text());
    }
    qDebug() << q;
    db->showView (q, ui->tableView);
}


void MainWindow::paint()
{
    QVector<QColor> colors = {
        QColor ("#f98f86"), QColor ("#88b861")
    };
    if (curAg == "") return;
    etalons = db->getListPar ("etalon");
    for (int i = 0; i < 7; ++i) {
        if (ui->tableWidget_2->item (i, 0) == nullptr) continue;
        if (ui->tableWidget_2->item (i, 0)->text() == "") {
            ui->tableWidget_2->item (i, 0)->setBackgroundColor (QColor ("white"));
        } else {
            ui->tableWidget_2->item (i, 0)->setBackgroundColor (colors[ui->tableWidget_2->item (i, 0)->text() == etalons[i]]);
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
    QString q = QString ("update info "
                         "set p%1 = '%2' "
                         "where agent = 'etalon'").arg (
                    QString::number (ui->comboBox->currentIndex() + 1),
                    ui->lineEdit_3->text()
                );
    db->exec (q);
    showEt();
    paint();
}

void MainWindow::on_pushButton_4_clicked()
{
    QString q = QString ("select agent 'Агент', p1 '%1', p2 '%2', p3 '%3', "
                         "p4 '%4', p5 '%5', p6 '%6',"
                         "p7 '%7' from info where agent <> 'etalon' ").arg (
                    params[0], params[1], params[2],
                    params[3], params[4], params[5],
                    params[6]
                );
    qDebug() << q;
    db->showView (q, ui->tableView);
}

void MainWindow::on_pushButton_3_clicked()
{
    showDB();
}

void MainWindow::on_pushButton_5_clicked()
{

}

void MainWindow::on_pushButton_6_clicked()
{
    QString q = "select p1, p2, p3, "
                "p4, p5, p6, p7, "
                " agent from info where agent <> 'etalon' ";

    if (ui->checkBox->isChecked()) {
        q += " and agent like '%" + ui->lineEdit_5->text() + "%' ";
    }
    if (ui->checkBox_2->isChecked()) {
        q += QString (" and p%1 like '%%2%'").arg (
                 QString::number (ui->comboBox_2->currentIndex() + 1),
                 ui->lineEdit_7->text());
    }
    qDebug() << q;
    db->createWord (q);
}

void MainWindow::on_listWidget_itemDoubleClicked (QListWidgetItem *item)
{
    if (item->text() == "") return;
    curAg = item->text();
    QJsonObject obj;
    obj["request"] = "info";
    int row = ui->listWidget->row (item);
    if (row < agents.size())
        emit sendMessage (JsonToString (&obj), agents[row]);
    showTw();
}

void MainWindow::on_pushButton_7_clicked()
{
    auto agent = db->analitic();
    if (agent != "")
        QMessageBox::about (this,
                            "Аналитическая функция.", QString (
                                "У Агента \"%1\" наибольшее отклонение от эталона."
                            ).arg (agent));
}

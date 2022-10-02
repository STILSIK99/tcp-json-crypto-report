#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow (QWidget *parent)
    : QMainWindow (parent)
    , ui (new Ui::MainWindow)
{
    ui->setupUi (this);
    server = new Server();
    connect (server, &Server::operationFinished, this, &MainWindow::slotOperationFinish);
    connect (this, &MainWindow::sendMessage, server, &Server::sendToClient);
    connect (server, &Server::recievedMessage, this, &MainWindow::slotProcess);
    connect (this, &MainWindow::makeConnection, server, &Server::slotMakeConnection);

    connect (ui->action, &QAction::triggered, this, &MainWindow::slotOpenDB);
    //create
    connect (ui->action_2, &QAction::triggered, this, &MainWindow::slotCreateDB);
    db = new DB();
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
    } else {
        QMessageBox::about (this, title, "Не подключено.");
    }
}

void MainWindow::slotSendRequest (bool result)
{
    if (result) {
        QMessageBox::about (this, "", "Запрос отправлен агенту.");
    } else {
        QMessageBox::about (this, "", "Не удалось отправить запрос агенту.");
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
    if (reply.contains ("request")) {
        if (reply["request"].toString() == "check") {
            if (reply.contains ("url"))
                if (reply["url"].isArray()) {
                    auto arr = reply["url"].toArray();
                    QJsonArray mas;
                    for (int i = 0; i < arr.size(); ++i) {
                        mas.append (check (arr[i].toObject()));
                        db->insertResult (mas[i].toObject(), agent);
                    }
                    on_pushButton_3_clicked();
                    reply["url"] = mas;
                    reply["reply"] = "check";
                    reply.take ("request");
                    sendMessage (JsonToString (&reply), agent);
                }
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

void MainWindow::on_pushButton_3_clicked()
{
    QString q = "select agent as 'Агент', URL, "
                "result as 'Результат проверки', date as 'Дата проверки' "
                "from history where 1 = 1  ";

    if (ui->checkBox->isChecked()) {
        q += " and agent like '%" + ui->lineEdit_3->text() + "%' ";
    }
    if (ui->checkBox_2->isChecked()) {
        q += " and url like '%" + ui->lineEdit_4->text() + "%' ";
    }
    if (ui->checkBox_3->isChecked()) {
        q += " and result like '%" + ui->lineEdit_5->text() + "%' ";
    }
    qDebug() << q;
    db->showView (q, ui->tableView);
}

void MainWindow::on_pushButton_2_clicked()
{
    QString q = "select agent as 'Агент', URL, "
                "result as 'Результат проверки', date as 'Дата проверки' "
                "from history where 1 = 1  ";

    if (ui->checkBox->isChecked()) {
        q += " and agent like '%" + ui->lineEdit_3->text() + "%' ";
    }
    if (ui->checkBox_2->isChecked()) {
        q += " and url like '%" + ui->lineEdit_4->text() + "%' ";
    }
    if (ui->checkBox_3->isChecked()) {
        q += " and result like '%" + ui->lineEdit_5->text() + "%' ";
    }
    qDebug() << q;
    db->getReport (q);
}

void MainWindow::on_pushButton_4_clicked()
{
    QString q = "select agent as 'Агент', URL, "
                "result as 'Результат проверки', date as 'Дата проверки' "
                "from history where 1 = 1  ";
    db->showView (q, ui->tableView);
}


QJsonObject MainWindow::check (QJsonObject obj)
{
    QRegExp rx ("^http:.*|"
                "(.*/.{1,}/.{1,}/.{1,}/.*)|"
                "(.*@.*)|"
                "(.*\\..*\\..*\\..*)|"
                "(.*=.*)|"
                "(.*-.*)$");
    QString url = obj["url"].toString();
    if (rx.exactMatch (url)) {
        obj["result"] = "Фишинговый сайт";
    } else {
        obj["result"] = "Легальный сайт";
    }
    return obj;
}

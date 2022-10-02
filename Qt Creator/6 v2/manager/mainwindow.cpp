#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QStyle>
#include <QAction>
#include <QTableWidget>
#include <QJsonDocument>
#include <QJsonObject>

const QColor orange = QColor (255, 170, 0);
const QColor red = QColor (213, 29, 57);
const QColor green = QColor (44, 197, 62);


MainWindow::MainWindow (QWidget *parent)
    : QMainWindow (parent)
    , ui (new Ui::MainWindow)
{
    isValid = false;
    ui->setupUi (this);
    tcp_c = new QTcpSocket (this);
    counter = 0;
    db = new DB;
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ipRegex ("^" + ipRange + "\\." + ipRange + "\\." + ipRange
                     + "\\." + ipRange + "$");
    ui->lineEdit_4->setValidator (new QRegExpValidator (ipRegex, this));
    //init server
    initServer (33333);
    //open
    connect (ui->action, &QAction::triggered, this, &MainWindow::slotOpenDB);
    //    //create
    connect (ui->action_2, &QAction::triggered, this, &MainWindow::slotCreateDB);
    //    //close app
    connect (ui->action_3, &QAction::triggered, this, &MainWindow::slotCloseApp);
    block();
    ui->dateTimeEdit->setDateTime (QDateTime::currentDateTime());
    ui->dateTimeEdit_2->setDateTime (QDateTime::currentDateTime());

    //    if (!readConfig()) {
    //        auto ans = QMessageBox::question (this, "Загрузка конфигурации.", "Файл конфигурации не найден.\nВыбрать файл конфигурации?");
    //        if (ans == QMessageBox::No) {
    //            isValid = false;
    //            return;
    //        };
    //        QString path = "";
    //        QFileDialog::getOpenFileName (this, "Выберите файл конфигурации:", QDir::currentPath(), "TextFile (*.txt)");
    //        if (path == "" || !readConfig (path)) {
    //            isValid = false;
    //            return;
    //        };
    //    }
    //    isValid = true;
}

bool MainWindow::readConfig (QString a)
{
    QFile cfg (a);
    if (!cfg.open (QFile::ReadOnly | QFile::Text))
        return false;
    QJsonObject obj = QJsonDocument().fromJson (cfg.readAll()).object();
    if (obj.contains ("decrypt key"))
        de_key = obj["decrypt key"].toString();
    else return false;
    if (obj.contains ("encrypt key"))
        en_key = obj["encrypt key"].toString();
    else return false;
    return true;
    //    if (obj.contains("port"))
    //        port = obj["port"].toInt();
}

void MainWindow::block()
{
    auto btns = ui->tabWidget->findChildren <QPushButton *>();
    for (auto btn : btns)
        btn->setEnabled (false);
}

void MainWindow::unblock()
{
    auto btns = ui->tabWidget->findChildren <QPushButton *>();
    for (auto btn : btns)
        btn->setEnabled (true);
}


void MainWindow::slotCloseApp()
{
    if (db->connected()) db->close();
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
        db->showEtalons (ui->tableWidget);
        db->showAgents (ui->tableWidget_2);
        ui->comboBox_2->clear();
        ui->comboBox_2->addItems (db->paramsNames.toList());
        unblock();
        QMessageBox::about (this, title, "Успешно.");
    } else {
        QMessageBox::about (this, title, "Не подключено.");
        block();
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
        ui->comboBox_2->clear();
        ui->comboBox_2->addItems (db->paramsNames.toList());
        db->showAgents (ui->tableWidget_2);
        db->showEtalons (ui->tableWidget);
        unblock();
        QMessageBox::about (this, title, "Успешно.");
    } else {
        block();
        QMessageBox::about (this, title, "Не подключено.");
    }
}


MainWindow::~MainWindow()
{
    for (auto x : ip_TPP.keys()) {
        if (!doConnect (x, 33334)) {
            QMessageBox::about (this, "Нет подключения!", "Узел недоступен!");
            return;
        }
        send_to_server (Request ("turn_off", "", "").createMessage(), x);
        auto it = ip_TPP.find (x);
        positions.erase (std::get<2> (*it));
        ip_TPP.erase (it);
    }
    tcp_s->close();
    delete tcp_s;
    delete tcp_c;
    delete ui;
    if (db != nullptr) delete db;
}


void MainWindow::on_pushButton_2_clicked()
{
    int row = -1;
    QString ip = ui->lineEdit_4->text();
    if (!doConnect (ip, 33334)) {
        QMessageBox::about (this, "Нет подключения!", "Узел недоступен!");
        return;
    }
    if (ip_TPP.count (ip) == 0) {
        ip_TPP[ip] = std::make_tuple (false, ui->spinBox->value(), counter);
        positions.insert (counter++);
        auto pos = positions.lower_bound (std::get<2> (ip_TPP[ip]));
        if (pos == positions.end()) {
            QMessageBox::about (this, "Error!", "Не найден индекс!");
            return;
        }
        row = std::distance (positions.begin(), pos);
    } else {
        QMessageBox::about (this, "Недопустимое действие!", "Данный агент уже добавлен!");
        return;
    }
    ui->tableWidget_3->setRowCount (ui->tableWidget_3->rowCount() + 1);
    auto item = new QTableWidgetItem (ip);
    item->setData (Qt::CheckStateRole, Qt::Unchecked);
    item->setFlags (item->flags() & (~Qt::ItemIsEditable));
    connect (ui->tableWidget_3, &QTableWidget::cellDoubleClicked, this, &MainWindow::tw3DblClk);
    ui->tableWidget_3->setItem (row, 0, item);
    ui->tableWidget_3->setItem (row, 1,
                                new QTableWidgetItem (QString::number (ui->spinBox->value())));
    ui->tableWidget_3->item (row, 1)->setFlags (item->flags() & (~Qt::ItemIsEditable));
    changeState (ip, 1);
    ui->tableWidget_3->resizeColumnsToContents();
    QJsonObject data;
    data["period"] = ui->spinBox->value();
    send_to_server (Request ("turn_on", "", JsonToString (data)).createMessage (), ip);
    ui->comboBox->addItem (ip);
}

void MainWindow::handler (Request r, QHostAddress agent)
{
    QString ip = QHostAddress (agent.toIPv4Address()).toString();
    //    ui->textEdit_2->append (r.createMessage());
    if (r.action == "pack" && r.table == "data") {
        changeState (ip, 2);
        if (db->connected()) db->addPackege (StringToJson (r.data), ip);
        db->showAgents (ui->tableWidget_2);
    }
    if (r.action == "turn_on" && r.data == "ok") {
        std::get<0> (ip_TPP[ip]) = true;
        changeState (ip, 2);
    }
    if (r.action == "turn_off") {
        changeState (ip, 0);
    }
    if (r.action == "changePeriod") {
        int row = changeState (ip, 0);
        if (row != -1) {
            auto it = ui->tableWidget_3->item (row, 1);
            try {
                int k = r.data.toInt();
                it->setText (r.data);
                std::get<1> (ip_TPP[ip]) = k;
            } catch (...) {}
        }
    }
}

int MainWindow::changeState (QString ip, int state)
{
    auto pos = positions.lower_bound (std::get<2> (ip_TPP[ip]));
    if (pos == positions.end()) {
        QMessageBox::about (this, "Error!", "Не найден индекс!");
        return -1;
    }
    int row = std::distance (positions.begin(), pos);
    QColor x;
    if (state == 0) x = red;
    if (state == 1) x = orange;
    if (state == 2) x = green;
    ui->tableWidget_3->item (row, 0)->setBackgroundColor (x);
    ui->tableWidget_3->item (row, 1)->setBackgroundColor (x);
    return row;
}

void MainWindow::on_pushButton_3_clicked()
{
    for (int rowId = 0; rowId < ui->tableWidget_3->rowCount(); ++rowId) {
        auto it = ui->tableWidget_3->item (rowId, 0);
        if (it->data (Qt::CheckStateRole) == Qt::Checked) {
            auto ip = it->text();
            if (ip_TPP.count (ip) == 0) return;
            auto pos = std::get<2> (ip_TPP[ip]);
            positions.erase (pos);
            ip_TPP.erase (ip_TPP.find (ip));
            if (!doConnect (ip, 33334)) {
                QMessageBox::about (this, "Нет подключения!", "Узел недоступен!");
                return;
            }
            send_to_server (Request ("turn_off", "", "").createMessage(), ip);
            ui->tableWidget_3->removeRow (rowId);
        }
    }
}


QJsonObject MainWindow::StringToJson (QString a)
{
    return QJsonDocument::fromJson (a.toUtf8()).object();
}

QString MainWindow::JsonToString (QJsonObject a)
{
    return QString (QJsonDocument (a).toJson (QJsonDocument::Compact));
}

void MainWindow::tw3DblClk (int row, int col)
{
    QString ip = ui->tableWidget_3->item (row, 0)->text();
    bool state = std::get<0> (ip_TPP[ip]);
    int period = std::get<1> (ip_TPP[ip]);
    if (state == true) {
        changeState (ip, 0);
        if (doConnect (ip, 33334)) {
            send_to_server (Request ("turn_off", "", "").createMessage(), ip);
        }
    } else {
        changeState (ip, 1);
        std::get<0> (ip_TPP[ip]) = 0;
        if (doConnect (ip, 33334)) {
            QJsonObject obj;
            obj["period"] = QString::number (period);
            send_to_server (Request ("turn_on", "", JsonToString (obj)).createMessage(), ip);
        }
    }
    std::get<0> (ip_TPP[ip]) = 0;
};


void MainWindow::on_pushButton_4_clicked()
{
    QString ip = ui->comboBox->currentText();
    int k = ui->spinBox_2->value();
    if (!doConnect (ip, 33334)) {
        QMessageBox::about (this, "Нет подключения!", "Узел недоступен!");
        return;
    }
    send_to_server (Request ("changePeriod", "", QString::number (k)).createMessage(), ip);
    changeState (ip, 1);
}

void MainWindow::on_pushButton_clicked()
{
    QString query = "select ip as 'Ip address', date as 'Datetime"
                    "', par1 as 'p1', par2 as 'p2', par3 as 'p3',"
                    " par4 as 'p4', par5 as 'p5', par6 as 'p6', par7 as 'p7' from data ";
    for (int i = 1; i < 8; ++i)
        query = query.replace ("p" + QString::number (i), db->paramsNames[i - 1]);

    QString add = "";
    if (ui->checkBox->isChecked()) {
        add += " ip like '" + ui->lineEdit->text() +  "'";
    }
    if (ui->checkBox_2->isChecked()) {
        if (add != "") add += " and ";
        add += "date between 'd1' and 'd2'";
        add = add.replace ("d1", ui->dateTimeEdit->text()).replace ("d2", ui->dateTimeEdit_2->text());
    }
    query += (add == "") ? "" : ("where " + add);
    qDebug() << query;
    db->showView (query, ui->tableView);
}

void MainWindow::on_pushButton_5_clicked()
{
    QVector<QString> agents;
    for (int i = 0; i < ui->tableWidget_2->rowCount(); ++i) {
        if (ui->tableWidget_2->item (i, 0)->data (Qt::CheckStateRole) == Qt::Checked)
            agents.push_back (ui->tableWidget_2->item (i, 0)->text());
    }
    if (agents.size() != 0)
        db->createReport (agents);
}

void MainWindow::on_pushButton_6_clicked()
{
    int ind = ui->comboBox_2->currentIndex();
    QVariant data;
    QString param = "par" + QString::number (ind + 1);
    if ((ind >= 0 && ind < 4) || ind == 6) {
        try {
            int k = ui->lineEdit_2->text().toInt();
            data = k;
        } catch (...) {
            QMessageBox::about (this, "Недопустимое значение.", "Неверно задано число");
            return;
        }
    } else if (ind > 3 && ind < 6) {
        data = ui->lineEdit_2->text();
    } else return;
    if (db->changeEtalon (data, param)) {
        QMessageBox::about (this, "Выполнено", "Значение изменено.");
    } else {
        QMessageBox::about (this, "Ошибка", "Операция не выполнена");
    }
    db->showEtalons (ui->tableWidget);
    ui->lineEdit_2->setText ("");
}

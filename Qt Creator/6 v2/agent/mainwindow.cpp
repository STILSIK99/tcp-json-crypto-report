#include "mainwindow.h"
#include "ui_mainwindow.h"

const QColor orange = QColor (255, 170, 0);
const QColor red = QColor (213, 29, 57);
const QColor green = QColor (44, 197, 62);

const int mnoj = 1000 * 10;

MainWindow::MainWindow (QWidget *parent)
    : QMainWindow (parent)
    , ui (new Ui::MainWindow)
{
    ui->setupUi (this);
    initServer (33334);
    tcp_c = new QTcpSocket (this);
    timer = new QTimer;
    connect (timer, &QTimer::timeout, this, &MainWindow::slotSendInfo);

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

void MainWindow::slotSendInfo ()
{
    QJsonObject *pack = GInfo();
    (*pack)["time"] = QDateTime::currentDateTime().toString ("dd.MM.yyyy hh:mm:ss");
    QString reply = JsonToString (*pack);
    Request repl ("pack", "data", reply);
    ui->textEdit->append ("reply: " + reply);
    if (!doConnect (ip_man, 33333)) {
        delete pack;
        timer->stop();
    } else
        send_to_server (repl.createMessage(), ip_man.toString());
    delete pack;
};

void MainWindow::exec (Request r)
{
    ui->textEdit->append (r.action + " " + r.table + " " + r.data);
    if (r.action == "turn_on") {
        QJsonObject data = StringToJson (r.data);
        qDebug() << r.data;
        if (!timer->isActive()) {
            delete timer;
            timer = new QTimer;
            connect (timer, &QTimer::timeout, this, &MainWindow::slotSendInfo);
            timer->setInterval (std::max ( mnoj, data["period"].toInt() * mnoj));
            timer->start ();
            qDebug() << timer->interval();
        }
        ui->textEdit->append ("period - " + QString::number (data["period"].toInt()));
        if (!doConnect (ip_man, 33333)) {
            QMessageBox::about (this, "Нет подключения!", "Узел недоступен!");
            timer->stop();
        }
        send_to_server (Request ("turn_on", "", "ok").createMessage (), ip_man.toString());
        QString ip = QHostAddress (ip_man.toIPv4Address()).toString();
        ui->lineEdit->setText ("Manager - " + ip);
        ui->lineEdit->setStyleSheet ("QLineEdit{ background-color: rgba(44, 197, 62, 80)}");

    }
    if (r.action == "turn_off") {
        timer->stop();
        ui->lineEdit->setText ("");
        ui->lineEdit->setStyleSheet ("QLineEdit{ background-color: rgba(213, 29, 57, 80)}");
    }
    if (r.action == "changePeriod") {
        if (timer->isActive()) {
            int k = -1;
            try {
                k = r.data.toInt();
                timer->setInterval (mnoj * k);
            } catch (...) {
                k = timer->interval() / mnoj;
            }
            if (!doConnect (ip_man, 33333)) {
                QMessageBox::about (this, "Нет подключения!", "Узел недоступен!");
                timer->stop();
                return;
            }
            send_to_server (Request ("changePeriod", "", QString::number (k)).createMessage (), ip_man.toString());
        }
    }
};

MainWindow::~MainWindow()
{
    if (doConnect (ip_man, 33333)) {
        send_to_server (Request ("turn_off", "", "").createMessage(), ip_man.toString());
    }
    delete tcp_s;
    delete tcp_c;
    delete ui;
}

QJsonObject MainWindow::StringToJson (QString a)
{
    return QJsonDocument::fromJson (a.toUtf8()).object();
}

QString MainWindow::JsonToString (QJsonObject a)
{
    return QString (QJsonDocument (a).toJson (QJsonDocument::Compact));
}


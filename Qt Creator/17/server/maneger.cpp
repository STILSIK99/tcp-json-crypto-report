#include "maneger.h"
#include "ui_maneger.h"

QStringList list1, list1info, list1proinfo;
QStringList derect1, dirinfo, dirproinfo;
QVector <QString> analit;
Maneger::Maneger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Maneger)
{
    ui->setupUi(this);
    server = new Server();
    connect (server, &Server::AgentDisconnected, this, &Maneger::slotAgentDisconnect);
    connect (server, &Server::AgentConnected, this, &Maneger::slotAgentConnect);
    connect (server, &Server::operationFinished, this, &Maneger::slotOperationFinish);
    connect (this, &Maneger::sendMessage, server, &Server::sendToClient);
    connect (server, &Server::recievedMessage, this, &Maneger::slotProcess);
    connect (this, &Maneger::makeConnection, server, &Server::slotMakeConnection);
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ipRegex ("^" + ipRange + "\\." + ipRange + "\\." + ipRange + "\\." + ipRange + "$");
    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
    ui->lineEdit->setValidator(ipValidator);
    ui->lineEdit_2->setValidator(new QIntValidator (1000, 99999, this));
    ui->tableWidget->setColumnCount(3);
    QStringList info;
    info << tr("Найденные файлы") << tr("Дата изменения") << tr("Дата последнего доступа");
    ui->tableWidget->setHorizontalHeaderLabels(info);
    ui -> tableWidget -> horizontalHeader() -> setStretchLastSection(true);
    ui->tableWidget->setColumnWidth(0, 600);
    ui->tableWidget->setColumnWidth(1, 100);
    ui -> tableWidget -> verticalHeader() -> setDefaultSectionSize(20);
    ui -> tableWidget -> setFrameShape(QFrame::NoFrame);
    ui -> tableWidget -> setShowGrid(true);
    pword = new QAxObject("Word.Application");
    pword->setProperty("Visible", true);
    QAxObject *pdoc = pword->querySubObject("Documents");
    pdoc = pdoc->querySubObject("Add()");
    QAxObject *pactive = pword->querySubObject("ActiveDocument()");
    prange = pactive->querySubObject("Range()");
    prange->setProperty("Text", "Данные для сохранения: \n");
    QFile file("BD.sqlite");
    file.remove();
}

Maneger::~Maneger()
{
    delete ui;
}

void Maneger::slotSendRequest (bool result)
{
    if (result) {
        QMessageBox::about (this, "", "Запрос отправлен агенту.");
    } else {
        QMessageBox::about (this, "", "Не удалось отправить запрос агенту.");
    }
}

void Maneger::slotAgentConnect (QPair<QString, int> agent)
{
    agents.append (agent);
}

void Maneger::slotAgentDisconnect (QPair<QString, int> agent)
{
    for (int i = 0; i < agents.size(); ++i) {
        if (agents[i] == agent) {
            agents.erase (agents.begin() + i);
            break;
        }
    }
}

void Maneger::slotOperationFinish (bool res, QPair<QString, int>)
{
    if (!res) {
        QMessageBox::about (this, "", "Операция не выполнена.");

    }
}

void Maneger::on_pushButton_clicked()
{
    QString ip = ui->lineEdit->text();
    int port = -1;
    try {
        port = ui->lineEdit_2->text().toInt();
    } catch (...) {
        QMessageBox::about (this, "", "Такого порта не существует!");
        return;
    }
    auto pair = qMakePair (ip, port);
    emit makeConnection (pair);
    curAg = pair;
}

void Maneger::slotProcess (QPair<QString, int> agent, QJsonObject reply)
{
    if (reply.contains ("All")) {
            appendline(reply, agent);
    }

}

void Maneger::appendline (QJsonObject &obj, QPair<QString, int> &agent)
{
    list1.clear();
    list1info.clear();
    list1proinfo.clear();
    int a = obj["All"].toString().toInt();
    for (int i = 0; i < a; i++)
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        QStringList  poi = obj[QString::number(i)].toString().split("%;%");
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(poi[0]));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(poi[1]));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(poi[2]));
        list1.append(poi[0]);
        list1info.append(poi[1]);
        list1proinfo.append(poi[2]);
    }
};

QString Maneger::JsonToString (QJsonObject *a)
{
    return QString (QJsonDocument (*a).toJson (QJsonDocument::Compact));
}

QJsonObject Maneger::StringToJson (QString a)
{
    return QJsonDocument::fromJson (a.toUtf8()).object();
}

void Maneger::on_pushButton_2_clicked()
{
    if (ui->lineEdit_3->text() != "")
    {
        QJsonObject obj;
        obj["put"]=ui->lineEdit_3->text();
        emit sendMessage (JsonToString (&obj), curAg);
        QMessageBox::about (this, "", "Запрос отправлен.");
        QString puti = ui->lineEdit_3->text();
        if (analit.size() == 0){
            analit.append(puti);
            ui->listWidget->addItem(puti + " " + "Количество доступов: " + QString::number(1));
        }
        else{
            int koi = 0;
            for (int i = 0; i < analit.size(); i++)
            {
                if (puti == analit.at(i))
                    koi++;
            }
            if (koi == 0){

                analit.append(puti);
                ui->listWidget->addItem(puti + " " +  "Количество доступов: " + QString::number(1));
            }
            else
            {
                analit.append(puti);
                int count = 0;
                for (int i = 0; i < analit.size(); i++)
                {
                    if (puti == analit.at(i))
                    {
                        count++;
                    }
                }
                for (int i = 0; i < ui->listWidget->count(); i++)
                {
                    if (ui->listWidget->item(i)->text().contains(puti)){
                        ui->listWidget->item(i)->setText(puti + " " + "Количество доступов: " + QString::number(count));
                    }
                }
                count = 0;
            }
        }
    }
    else{
        QMessageBox::about (this, "", "Введите директорию поиска.");
    }
}

void Maneger::on_pushButton_3_clicked()
{
    QString zapis;
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->clear();
    QStringList list;
    list << tr("Найденные файлы") << tr("Дата изменения") << tr("Дата последнего доступа");
    ui->tableWidget->setHorizontalHeaderLabels(list);
    QString date1 = ui->dateTimeEdit->dateTime().toString();
    QString date2 = ui->dateTimeEdit_2->dateTime().toString();
    for (int i = 0; i < list1.size(); i++)
    {
        if (((list1info[i] >= date1 ) && (list1info[i] <= date2)) || ((list1proinfo[i] >= date1) && (list1proinfo[i] <= date2)))
        {
            derect1 << list1[i];
            dirinfo << list1info[i];
            dirproinfo << list1proinfo[i];
        }
    }
    for (int i = 0; i < derect1.size(); i++)
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(derect1[i]));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(dirinfo[i]));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(dirproinfo[i]));
        zapis = derect1[i] + " " + dirinfo[i] + " " + dirproinfo[i];
        prange->dynamicCall("InsertAfter(Text)", zapis+"\n");
    }
    static int i = 1;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("BD.sqlite");
    if (!db.open()) {
        qDebug() << "Cannot open database:" << db.lastError();
    }
    QSqlQuery query;
    QString   str  = "CREATE TABLE BD ( "
                  "id INTEGER PRIMARY KEY NOT NULL, "
                  "fail VARCHAR(100),"
                  "data_1 VARCHAR(100), "
                  "data_2 VARCHAR(100)"
                  ");";

    if (!query.exec(str)) {
        qDebug() << "Unable to create a table";
    }
    QString strF = "INSERT INTO BD (id, fail, data_1, data_2) "
                   "VALUES(%1, '%2', '%3', '%4');";
    for (int j = 0; j < ui->tableWidget->rowCount(); j++)
    {
        str = strF.arg(j+i).arg(ui->tableWidget->item(j,0)->text()).arg(ui->tableWidget->item(j,1)->text()).arg(ui->tableWidget->item(j,2)->text());

        if (!query.exec(str)) {
            qDebug() << "Unable to do insert opeation";
        }
    }

    if (!query.exec("SELECT * FROM INFO;")) {
        qDebug() << "Unable to execute query - exiting";
    }
}

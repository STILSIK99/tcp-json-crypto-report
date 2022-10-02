#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow (QWidget *parent)
    : QMainWindow (parent)
    , ui (new Ui::MainWindow)
{
    ui->setupUi (this);
    //    curPath = QDir::current().path();
    //    socket = new QTcpSocket (this);
    //    connect (socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    //    connect (socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    //    nextBlockSize = 0;
    //    directory = "";
    //    currentScans.clear();
    //    ui->lineEdit_3->setValidator (
    //        new QRegExpValidator (
    //            QRegExp ("^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$")
    //        ));
    //    ui->lineEdit_4->setValidator (
    //        new QRegExpValidator (
    //            QRegExp ("^[\d]+$")
    //        ));

}



MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_checkBox_clicked()
{
    //    if (ui->checkBox->isChecked()) {
    //        ui->checkBox->setText ("не отправлять");
    //    } else ui->checkBox->setText ("отправлять");
}

void MainWindow::on_pushButton_clicked()
{
    //    QString path = QFileDialog::getExistingDirectory (
    //                       this,
    //                       "Выберите директорию с лог-файлами",
    //                       curPath,
    //                       QFileDialog::ReadOnly | QFileDialog::ShowDirsOnly);
    //    if (path == "") {
    //        ui->label->setText ("Директория поиска");
    //        return;
    //    }
    //    ui->label->setText (path);
    //    directory = path;

}

void MainWindow::on_pushButton_3_clicked()
{
    //    if (!findInTree (ui->lineEdit_2->text())) {
    //        QMessageBox::about (this, programName, "По вашему запросу ничего не найдено.");
    //    }
}





void MainWindow::on_pushButton_2_clicked()
{
    //    if (ui->checkBox->isChecked()) {
    //        if (socket->waitForConnected (1000)) {
    //            currentScans[directory] = true;
    //        }

    //    } else {
    //        currentScans[directory] = false;
    //    }

    //    directory = "";
}

void MainWindow::appendResultToTree (QJsonObject *obj)
{
    //    auto parent = new QTreeWidgetItem();
    //    ui->treeWidget->addTopLevelItem (parent);
    //    parent->setText (0, (*obj)["dir"].toString());
    //    parent->setText (1, (*obj)["reg"].toString());
    //    int sum = 0;
    //    auto mas = (*obj)["results"].toArray();
    //    for (auto el = mas.begin(); el != mas.end(); ++el) {
    //        auto e = el->toObject();
    //        auto node = new QTreeWidgetItem();
    //        node->setText (2, e["file"].toString());
    //        node->setText (3, QString::number (e["count"].toInt()));
    //        parent->addChild (node);
    //    }
}

void MainWindow::on_pushButton_4_clicked()
{
    //    bool request = false;
    //    int port = -1;
    //    try {
    //        port = ui->lineEdit_4->text().toInt();
    //    } catch (...) {
    //        request = true;
    //    }
    //    if (request || port > 65535) {
    //        QMessageBox::about (
    //            this,
    //            programName,
    //            "Неверно заданы параеметры \nподлкючения к менеджеру.");
    //        return;
    //    }
    //    socket->connectToHost (ui->lineEdit_3->text(), port);
    //    if (socket->waitForConnected (1000)) {
    //        ui->groupBox_2->setEnabled (false);
    //    } else {
    //        QMessageBox::about (this, programName, "Менеджер не доступен.");
    //    }
}































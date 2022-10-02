#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow (QWidget *parent)
    : QMainWindow (parent)
    , ui (new Ui::MainWindow)
{
    ui->setupUi (this);
    agent = new Agent;
    connect (agent, &Agent::showResult, this, &MainWindow::showResult);
    connect (this, &MainWindow::sendRequest, agent, &Agent::sendRequest);
    ui->tableWidget->setColumnCount (2);
    ui->tableWidget->setHorizontalHeaderLabels ({"URL", "Результат проверки"});
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    emit sendRequest (ui->textEdit->toPlainText());
    ui->textEdit->clear();
}

void MainWindow::showResult (QJsonArray obj)
{
    int row = ui->tableWidget->rowCount();
    for (int i = 0; i < obj.size(); ++i) {
        auto x = obj[i].toObject();
        if (x.contains ("url") && x.contains ("result")) {
            if (x["url"].isString() && x["result"].isString()) {
                ui->tableWidget->setRowCount (row + 1);
                ui->tableWidget->setItem (row, 0, new QTableWidgetItem (x["result"].toString()));
                ui->tableWidget->setItem (row, 1, new QTableWidgetItem (x["url"].toString()));
                ++row;
            }
        }
    }
    ui->tableWidget->resizeColumnsToContents();
}

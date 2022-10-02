#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QDataStream>
#include <QByteArray>
#include "parser.h"
#include <QThread>
#include <QRegExpValidator>
#include <QMessageBox>
#include <QTreeWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        MainWindow (QWidget *parent = nullptr);
        ~MainWindow();


    private slots:
        void on_checkBox_clicked();

        void on_pushButton_clicked();

        void on_pushButton_2_clicked();

        void on_pushButton_3_clicked();

        void on_pushButton_4_clicked();


    private:
        Ui::MainWindow *ui;
        QString curPath;
        //parser parameters
        QString directory;
        QMap<QString, bool> currentScans;
        //converter
        QJsonObject StringToJson (QString a);
        QString JsonToString (QJsonObject *a);
        //network
        QTcpSocket *socket;
        QByteArray data;
        quint16 nextBlockSize;
        QString programName;
        //visual
        void appendResultToTree (QJsonObject * );





};
#endif // MAINWINDOW_H

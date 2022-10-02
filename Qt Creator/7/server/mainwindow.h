#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QRegExpValidator>
#include <QRegExp>

#include <QJsonArray>
#include <QFileDialog>
#include "server.h"
#include <database.h>
#include <QAxObject>

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
        bool createDataBase (QString path);
        Server *server;
        //request
        QString ip;
        //database
        DB *db;


    private slots:

        void on_pushButton_clicked();

        QJsonObject StringToJson (QString a);

        QString JsonToString (QJsonObject *a);

        void appendTree (QJsonObject &, QPair<QString, int> &);

        void on_pushButton_2_clicked();


        void slotOpenDB();

        void slotCreateDB();

        void on_pushButton_4_clicked();

        void on_pushButton_3_clicked();

        void on_pushButton_6_clicked();

        void on_pushButton_5_clicked();

        void on_pushButton_7_clicked();

    public slots:
        void slotProcess (QPair<QString, int>, QJsonObject);
        void checkEtalon();


    private:
        Ui::MainWindow *ui;

    signals:
        void makeConnection (QPair<QString, int>);
};
#endif // MAINWINDOW_H

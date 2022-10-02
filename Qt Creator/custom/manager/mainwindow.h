#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
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
        Server *server;
        DB *db;

        QJsonObject StringToJson (QString a);

        QString JsonToString (QJsonObject *a);

        void slotOpenDB();

        void slotCreateDB();
        void slotSendRequest (bool);
        void slotOperationFinish (bool, QPair<QString, int>);
        void slotProcess (QPair<QString, int>, QJsonObject);
        bool createDataBase (QString path);
        QJsonObject check (QJsonObject );

    private slots:
        void on_pushButton_clicked();

        void on_pushButton_3_clicked();

        void on_pushButton_2_clicked();

        void on_pushButton_4_clicked();

    private:
        Ui::MainWindow *ui;

    signals:
        void makeConnection (QPair<QString, int>);
        void sendMessage (QString, QPair<QString, int>);
};
#endif // MAINWINDOW_H

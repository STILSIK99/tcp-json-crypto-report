#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
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
        QVector<QPair<QString, int>> agents;
        //request
        QString ip;
        QString directory;
        //database
        DB *db;


    private slots:
        void on_listWidget_itemDoubleClicked (QListWidgetItem *item);

        void on_pushButton_clicked();

        QJsonObject StringToJson (QString a);

        QString JsonToString (QJsonObject *a);
        void findItem (QString, QJsonObject &);
        void buildTree (QTreeWidgetItem *, QJsonObject &);
        void appendTree (QJsonObject &, QPair<QString, int> &);


        void on_treeWidget_itemDoubleClicked (QTreeWidgetItem *item, int column);

        void on_pushButton_2_clicked();

        void on_treeWidget_itemClicked (QTreeWidgetItem *item, int column);

        void slotOpenDB();

        void slotCreateDB();

        void on_pushButton_4_clicked();

        void on_pushButton_5_clicked();

    public slots:
        void slotSendRequest (bool);
        void slotAgentConnect (QPair<QString, int>);
        void slotAgentDisconnect (QPair<QString, int>);
        void slotOperationFinish (bool);
        void slotProcess (QPair<QString, int>, QJsonObject);


    private:
        Ui::MainWindow *ui;
    signals:
        void getTree (QPair<QString, int>, QString);
        void makeConnection (QPair<QString, int>);
        void scanDir (QPair<QString, int>, QString, QString );
};
#endif // MAINWINDOW_H

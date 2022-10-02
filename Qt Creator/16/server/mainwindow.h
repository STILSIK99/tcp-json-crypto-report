#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
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
        bool createDataBase (QString path);
        Server *server;
        QVector<QPair<QString, int>> agents;
        //database
        DB *db;
        QString curAg;
        int curBlock;
        QString curFile;
        int maxBlock;
        QMap<QPair<QPair<QString, int>, QString>, QTreeWidgetItem *> table;

    private slots:

        void on_pushButton_clicked();

        QJsonObject StringToJson (QString a);

        QString JsonToString (QJsonObject *a);

        void appendTree (QJsonObject &, QPair<QString, int> &);

        void showRead ();

        void on_treeWidget_itemDoubleClicked (QTreeWidgetItem *item, int column);

        void on_pushButton_2_clicked();

        void slotOpenDB();

        void slotCreateDB();

        void loadFromDB (QString, QString, int);

        void on_pushButton_4_clicked();

        void on_pushButton_5_clicked();

        void on_pushButton_3_clicked();

        void on_pushButton_6_clicked();

        void on_pushButton_8_clicked();

        void on_pushButton_7_clicked();

        void on_tableWidget_cellDoubleClicked (int row, int column);

    public slots:
        void slotSendRequest (bool);
        void slotAgentConnect (QPair<QString, int>);
        void slotAgentDisconnect (QPair<QString, int>);
        void slotOperationFinish (bool, QPair<QString, int>);
        void slotProcess (QPair<QString, int>, QJsonObject);


    private:
        Ui::MainWindow *ui;
    signals:
        void makeConnection (QPair<QString, int>);
        void sendMessage (QString, QPair<QString, int>);
};
#endif // MAINWINDOW_H

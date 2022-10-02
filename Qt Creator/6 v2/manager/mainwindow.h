#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <utility>
#include <set>
#include <QMainWindow>
#include <QRegExpValidator>
#include "parser.h"
#include <QMessageBox>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTableWidgetItem>
#include <QFileDialog>
#include "DataBase.h"
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

        QMap<QString, std::tuple<bool, int, int>> ip_TPP;
        //ip - (turn, period, position)
        int counter;
        bool isValid;
        std::set<int> positions;

        //CRYPTO
        QString de_key;
        QString en_key;
        //

        //TCP SERVER
        QTcpServer *tcp_s;
        int tcp_sPort;
        quint16 buf_s;
        void handler (Request, QHostAddress);
        //

        //TCP CLIENT
        quint16 buf_c;
        QTcpSocket *tcp_c;
        bool doConnect (QString host, int port);
        //

        //DB
        DB *db;

        //Table_3
        int changeState (QString, int);

        QString JsonToString (QJsonObject a);

        QJsonObject StringToJson (QString a);

    public slots:
        void on_pushButton_2_clicked();

        //menubar
        void slotCreateDB();
        void slotOpenDB();
        void slotCloseApp();

        //TCP Client
        void slotConnected();
        void slotReadyRead();
        void slotError (QAbstractSocket::SocketError);
        void send_to_server (QString, QString );
        //

        //TCP SERVER
        bool initServer (int);
        void slotNewConnection();
        void slotReadClient();
        void sendToClient (QTcpSocket *, QString &);
        //

        void block();
        void unblock();
        bool readConfig (QString a = QDir::currentPath() + "/config.txt");

        //tableWidgets
        void tw3DblClk (int, int);

        void on_pushButton_3_clicked();

        void on_pushButton_4_clicked();

    private slots:
        void on_pushButton_clicked();

        void on_pushButton_5_clicked();


        void on_pushButton_6_clicked();

    private:
        Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

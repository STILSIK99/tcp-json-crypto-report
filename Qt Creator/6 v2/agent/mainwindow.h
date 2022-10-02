#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QRegExpValidator>
#include "parser.h"
#include <QMessageBox>
#include <QTcpSocket>
#include <QTcpServer>
#include "info.h"
#include <QTimer>
#include <QDateTime>
#include <QDir>
#include <QFileDialog>

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

        Ui::MainWindow *ui;
        bool isValid;

        //CRYPTO
        QString de_key;
        QString en_key;
        //

        //TCP SERVER
        QTcpServer *tcp_s;
        int tcp_sPort;
        quint16 buf_s;
        QHostAddress ip_man;
        void exec (Request);
        bool initServer (int);

        //getter
        QTimer *timer;
        //


        //TCP CLIENT
        quint16 buf_c;
        QTcpSocket *tcp_c;
        bool doConnect (QHostAddress host, int port);
        void sendToClient (QTcpSocket *, QString &);
        //

        QString JsonToString (QJsonObject a);

        QJsonObject StringToJson (QString a);

        bool readConfig (QString a = QDir::currentPath() + "/config.txt");

    public slots:
        //TCP Client
        void slotConnected();
        void slotReadyRead();
        void slotError (QAbstractSocket::SocketError);
        void send_to_server (QString, QString);
        //

        //TCP SERVER
        void slotNewConnection();
        void slotReadClient();
        //

        //getter
        void slotSendInfo ();

    signals:
        void changePeriod (int);
        void stopGetter();
};
#endif // MAINWINDOW_H

#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>


class Server : public QTcpServer
{
        Q_OBJECT

    public:
        Server();
        QTcpSocket *socket;

    private:
        QMap<QPair<QString, int>, QTcpSocket *> sockets;
        QByteArray data;
        void sendToClient (QString, QPair<QString, int> );
        quint16 nextBlockSize;
        QJsonObject StringToJson (QString a);
        QString JsonToString (QJsonObject *a);

    public slots:
        void incomingConnection (qintptr socketDescriptor);
        void slotReadyRead();
        void slotMakeConnection (QPair<QString, int>);
        void slotDisconnect();
        void slotGetTree (QPair<QString, int>, QString);
        void slotScanDir (QPair<QString, int> agent, QString dir, QString reg);

    signals:
        void operationFinished (bool);
        void AgentConnected (QPair<QString, int>);
        void AgentDisconnected (QPair<QString, int>);
        void recievedMessage (QPair<QString, int>, QJsonObject);

};


#endif // SERVER_H

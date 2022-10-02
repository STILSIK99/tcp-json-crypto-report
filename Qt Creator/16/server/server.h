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
        quint16 nextBlockSize;
        QJsonObject StringToJson (QString a);
        QString JsonToString (QJsonObject *a);

    public slots:
        void sendToClient (QString, QPair<QString, int> );
        void incomingConnection (qintptr socketDescriptor);
        void slotReadyRead();
        void slotMakeConnection (QPair<QString, int>);
        void slotDisconnect();

    signals:
        void operationFinished (bool, QPair<QString, int>);
        void AgentConnected (QPair<QString, int>);
        void AgentDisconnected (QPair<QString, int>);
        void recievedMessage (QPair<QString, int>, QJsonObject);

};


#endif // SERVER_H

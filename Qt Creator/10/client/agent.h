#ifndef AGENT_H
#define AGENT_H

#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QDataStream>
#include <QByteArray>
#include "parser.h"
#include <QThread>

class Agent : public QTcpServer
{
        Q_OBJECT

    public:
        Agent();
        QTcpSocket *socket;

    private:
        QByteArray data;
        quint16 nextBlockSize;
        QJsonObject StringToJson (QString a);
        QString JsonToString (QJsonObject *a);
        void startMonitor (QString, int);
        QMap<QString, Parser *> stack;
        void getVolumeNames();

    public slots:
        void incomingConnection (qintptr socketDescriptor);
        void slotReadyRead();

        void sendToManager (QString);

        void parseRequest (QString req);

        void slotGetResult (QJsonObject odj);


};


#endif // SERVER_H
#endif // AGENT_H

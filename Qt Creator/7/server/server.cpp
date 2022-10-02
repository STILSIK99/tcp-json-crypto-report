#include "server.h"

Server::Server()
{
    nextBlockSize = 0;
}

void Server::incomingConnection (qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor (socketDescriptor);
    connect (socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect (socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    qDebug() << "client connected " << socketDescriptor;
}

void Server::slotReadyRead()
{
    socket = (QTcpSocket *)sender();
    QDataStream in (socket);
    in.setVersion (QDataStream::Qt_5_3);
    if (in.status() == QDataStream::Ok) {
        qDebug() << "read...";
        for (;;) {
            if (nextBlockSize == 0) {
                if (socket->bytesAvailable() < 2) {
                    break;
                }
                in >> nextBlockSize;
            }
            if (socket->bytesAvailable() < nextBlockSize) {
                break;
            }
            QString str;
            in >> str;
            nextBlockSize = 0;
            QJsonObject obj = StringToJson (str);
            for (auto x = sockets.begin(); x != sockets.end(); ++x) {
                if (x.value() == socket) {
                    emit recievedMessage (x.key(), obj);
                    socket->disconnectFromHost();
                    sockets.erase (x);
                    break;
                }
            }
            break;
        }
    } else {
        qDebug() << "DataStream error";
    }
}

void Server::slotMakeConnection (QPair<QString, int> agent)
{
    qDebug() << "slotMakeConnection";
    QTcpSocket *socket = new QTcpSocket;
    socket->connectToHost (agent.first, agent.second);
    if (socket->waitForConnected (1000)) {
        qDebug() << "Connected";
        connect (socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
        connect (socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
        sockets[agent] = socket;
        QJsonObject *obj = new QJsonObject;
        (*obj)["request"] = "scan";
        sendToClient (JsonToString (obj), agent);
    } else {
        qDebug() << "Not connected.";
    }
}

void Server::sendToClient (QString str, QPair<QString, int> agent)
{
    qDebug() << str;
    if (sockets.count (agent) == 0) {
        //        emit operationFinished (false);
        return;
    }
    auto socket = sockets[agent];
    data.clear();
    QDataStream out (&data, QIODevice::WriteOnly);
    out.setVersion (QDataStream::Qt_5_3);
    out << quint16 (0) << str;
    out.device()->seek (0);
    out << quint16 (data.size() - sizeof (quint16));
    socket->write (data);
    //    emit operationFinished (true);
}

QString Server::JsonToString (QJsonObject *a)
{
    return QString (QJsonDocument (*a).toJson (QJsonDocument::Compact));
}

QJsonObject Server::StringToJson (QString a)
{
    return QJsonDocument::fromJson (a.toUtf8()).object();
}

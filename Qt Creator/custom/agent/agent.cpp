#include "agent.h"

Agent::Agent()
{
    if (listen (QHostAddress::Any, 55555)) {
        qDebug() << "start";
    } else {
        qDebug() << "error";
    }
    nextBlockSize = 0;
    socket = new QTcpSocket;
    arr = new QJsonArray;
}

void Agent::incomingConnection (qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor (socketDescriptor);
    connect (socket, &QTcpSocket::readyRead, this, &Agent::slotReadyRead);
    connect (socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    qDebug() << "client connected " << socketDescriptor;
    QJsonObject obj;
    obj["request"] = "check";
    obj["url"] = *arr;
    sendToManager (JsonToString (&obj));
    arr = new QJsonArray;

}

void Agent::parseRequest (QString req)
{
    QJsonObject *reply = new QJsonObject;
    QJsonObject request = StringToJson (req);
    if (request.contains ("reply")) {
        if (request["reply"].toString() == "check") {
            if (request.contains ("url")) {
                if (request["url"].isArray()) {
                    emit showResult (request["url"].toArray());
                }
            }
        }
    }
}

void Agent::slotReadyRead()
{
    socket = (QTcpSocket *)sender();
    QDataStream in (socket);
    in.setVersion (QDataStream::Qt_5_3);
    if (in.status() == QDataStream::Ok) {
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
            qDebug() << "read...";
            QString str;
            in >> str;
            qDebug() << str;
            nextBlockSize = 0;
            parseRequest (str);
        }
    } else {
        //        ui->textBrowser->append ("DataStream error");
    }
}

void Agent::sendToManager (QString str)
{
    if (!socket->waitForConnected (1000)) {
        return;
    }
    data.clear();
    QDataStream out (&data, QIODevice::WriteOnly);
    out.setVersion (QDataStream::Qt_5_3);
    out << quint16 (0) << str;
    socket->write (data);
    out.device()->seek (0);
    out << quint16 (data.size() - quint16 (sizeof (quint16)));
}

void Agent::sendRequest (QString str)
{
    QJsonObject url;
    url["url"] = str;
    arr->append (url);
    try {
        if (socket->state() != QAbstractSocket::ConnectedState)
            return;
        if (!socket->waitForConnected (100))
            return;

    } catch (...) {
        socket = new QTcpSocket;
        return;
    }
    QJsonObject obj;
    obj["request"] = "check";
    obj["url"] = *arr;
    sendToManager (JsonToString (&obj));
    arr = new QJsonArray;
}

QString Agent::JsonToString (QJsonObject *a)
{
    return QString (QJsonDocument (*a).toJson (QJsonDocument::Compact));
}

QJsonObject Agent::StringToJson (QString a)
{
    return QJsonDocument::fromJson (a.toUtf8()).object();
}

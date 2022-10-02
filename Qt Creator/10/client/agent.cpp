#include "agent.h"
#include <QStorageInfo>

Agent::Agent()
{
    if (this->listen (QHostAddress::Any, 55555)) {
        qDebug() << "start";
    } else {
        qDebug() << "error";
    }
    nextBlockSize = 0;
}

void Agent::incomingConnection (qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor (socketDescriptor);
    connect (socket, &QTcpSocket::readyRead, this, &Agent::slotReadyRead);
    connect (socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    qDebug() << "client connected " << socketDescriptor;
}

void Agent::parseRequest (QString req)
{
    QJsonObject *reply = new QJsonObject;
    QJsonObject request = StringToJson (req);
    if (request.contains ("request")) {
        if (request["request"].toString() == "volume") {
            if (request.contains ("period") && request.contains ("volume")) {
                if (request["period"].isDouble() && request["volume"].isString())
                    startMonitor (request["volume"].toString(), request["period"].toInt());
            }
            if (request.contains ("volume")) {
                if (request["volume"].isString()) {
                    if (stack.count (request["volume"].toString()) != 0) {
                        stack[request["volume"].toString()]->work = false;
                        stack.remove (request["volume"].toString());
                    }
                }
            }
        }
        if (request["request"].toString() == "scan") {
            getVolumeNames();
        }
    }
    sendToManager (JsonToString (reply));
    delete reply;
}

void Agent::getVolumeNames()
{
    Parser *p = new Parser ();
    QThread *q = new QThread ();
    p->moveToThread (q);
    connect (q, &QThread::started, p, &Parser::getVolumesList);
    connect (p, &Parser::getResult, this, &Agent::slotGetResult);
    connect (q, &QThread::finished, p, &Parser::deleteLater);
    connect (q, &QThread::finished, q, &QThread::deleteLater);
    q->start();
}

void Agent::startMonitor (QString vol, int period)
{
    //start scan in thread
    Parser *p = new Parser ();
    if (stack.count (vol) != 0) {
        stack[vol]->work = false;
        stack.remove (vol);
    }
    p->period = period;
    p->work = true;
    p->vol = vol;
    stack[vol] = p;
    QThread *q = new QThread ();
    p->moveToThread (q);
    connect (q, &QThread::started, p, &Parser::thread);
    connect (p, &Parser::getResult, this, &Agent::slotGetResult);
    connect (q, &QThread::finished, p, &Parser::deleteLater);
    connect (q, &QThread::finished, q, &QThread::deleteLater);
    q->start();
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
    if (socket == NULL) return;
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

void Agent::slotGetResult (QJsonObject obj)
{
    qDebug() << "Result was get";
    qDebug() << JsonToString (&obj);
    sendToManager (JsonToString (&obj));
}

QString Agent::JsonToString (QJsonObject *a)
{
    return QString (QJsonDocument (*a).toJson (QJsonDocument::Compact));
}

QJsonObject Agent::StringToJson (QString a)
{
    return QJsonDocument::fromJson (a.toUtf8()).object();
}

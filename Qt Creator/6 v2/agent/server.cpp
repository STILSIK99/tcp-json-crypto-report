#include "mainwindow.h"

bool MainWindow::initServer (int port)
{
    tcp_s = new QTcpServer;
    if (!tcp_s->listen (QHostAddress::Any, port)) {
        tcp_s->close();
        return false;
    }
    tcp_sPort = port;
    connect (tcp_s, SIGNAL (newConnection()), this, SLOT (slotNewConnection()));
    buf_s = 0;
    return true;
};

void MainWindow::slotNewConnection()
{
    QTcpSocket *sock = tcp_s->nextPendingConnection();
    connect (sock, SIGNAL (readyRead()),
             this, SLOT (slotReadClient()));
    connect (sock, SIGNAL (disconnected()),
             sock, SLOT (deleteLater()));
};

void MainWindow::slotReadClient()
{
    QTcpSocket *sock = (QTcpSocket *)sender();
    ip_man = sock->peerAddress();
    QDataStream in (sock);
    in.setVersion (QDataStream::Qt_5_7);
    while (true) {
        if (!buf_s) {
            if (sock->bytesAvailable() < sizeof (quint16)) break;
            in >> buf_s;
        }
        if (sock->bytesAvailable() <= buf_s) break;
    }
    QString str;
    in >> str;
    buf_s = 0;
    exec (Request (str));
}

void MainWindow::sendToClient (QTcpSocket *sock, QString &str)
{
    //    QString en_str = encrypt (str);
    QByteArray arrBlock;
    QDataStream out (&arrBlock, QIODevice::WriteOnly);
    out.setVersion (QDataStream::Qt_5_7);
    out << quint16 (0) << str;
    out.device()->seek (0);
    out << (quint16 (arrBlock.size()) - sizeof (quint16));
    sock->write (arrBlock);
}


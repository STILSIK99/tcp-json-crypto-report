#include "mainwindow.h"

bool MainWindow::doConnect (QString host, int port)
{
    buf_c = 0;
    tcp_c->connectToHost (host, port);
    if (tcp_c->state() != QTcpSocket::ConnectingState) return false;
    connect (tcp_c, SIGNAL (connected()), SLOT (slotConnected()));
    connect (tcp_c, SIGNAL (readyRead()), SLOT (slotReadyRead()));
    connect (tcp_c, SIGNAL (error (QAbstractSocket::SocketError)),
             this,         SLOT (slotError (QAbstractSocket::SocketError)));
    return true;
};

void MainWindow::slotReadyRead()
{
    //    QTcpSocket *sock = (QTcpSocket *)sender();
    //    QString ipv4 = QHostAddress (sock->peerAddress().toIPv4Address()).toString();
    //    QDataStream in (tcp_c);
    //    in.setVersion (QDataStream::Qt_5_7);
    //    for (;;) {
    //        if (!buf_c) {
    //            if (tcp_c->bytesAvailable() < sizeof (quint16))break;
    //            in >> buf_c;
    //        }
    //        if (tcp_c->bytesAvailable() <= buf_c) break;
    //        QString str = decrypt (str, );
    //        buf_c = 0;
    //    }
}

void MainWindow::slotError (QAbstractSocket::SocketError err)
{
    QString strError =
        "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                     "The host was not found." :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     "The remote host is closed." :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     "The connection was refused." :
                     QString (tcp_c->errorString()));
}

void MainWindow::send_to_server (QString str, QString ip)
{
    qDebug() << str << "\n";
    //    QString en_str = encrypt (str);
    QByteArray  arrBlock;
    QDataStream out (&arrBlock, QIODevice::WriteOnly);
    out.setVersion (QDataStream::Qt_5_7);
    out << quint16 (0) << str;
    out.device()->seek (0);
    out << quint16 (arrBlock.size() - sizeof (quint16));
    tcp_c->waitForBytesWritten();
    tcp_c->write (arrBlock);
    tcp_c->disconnectFromHost();
}

void MainWindow::slotConnected() {}

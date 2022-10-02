#include "mainwindow.h"
#include "parser.h"


bool MainWindow::doConnect (QHostAddress host, int port)
{
    host = QHostAddress (host.toIPv4Address());
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
    //    QDataStream in (tcp_c);
    //    in.setVersion (QDataStream::Qt_5_7);
    //    for (;;) {
    //        if (!buf_c) {
    //            if (tcp_c->bytesAvailable() < sizeof (quint16))break;
    //            in >> buf_c;
    //        }
    //        if (tcp_c->bytesAvailable() <= buf_c) break;
    //        QString str;
    //        in >> str;
    //        str = decrypt (str);
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
                     QString (tcp_c->errorString())
                    );
    qDebug() << strError << "\n";
}

void MainWindow::send_to_server (QString str, QString ip)
{
    qDebug() << str << "\n";
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

void MainWindow::slotConnected()
{
    //    te->append ("Received the connected() signal");
    //    qDebug() << "Received the connected() signal";
}

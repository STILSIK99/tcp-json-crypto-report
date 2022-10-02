#include "mainwindow.h"
#include "parser.h"

QStringList list1;
Parser::Parser () {}
QString MainWindow::JsonToString (QJsonObject *a)
{
    return QString (QJsonDocument (*a).toJson (QJsonDocument::Compact));
}
QJsonObject MainWindow::StringToJson (QString a)
{
    return QJsonDocument::fromJson (a.toUtf8()).object();
}

void Parser::slotGetSystemInfo ()
{
    list1.clear();
    QJsonObject obj;
    QDirIterator it(str, QStringList() << "*.*", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
        {
            QString put = it.next();
            QFileInfo file(put);
            QString save = put + "%;%" + file.lastModified().toString() + "%;%" + file.lastRead().toString();
            list1 << save;

        }
    obj["All"] = QString::number(list1.size());
    for (int i = 0; i < list1.size(); i++)
    {
        obj[QString::number(i)] = list1.at(i);
    }
    qDebug() << obj;
    emit getResult (obj);
}


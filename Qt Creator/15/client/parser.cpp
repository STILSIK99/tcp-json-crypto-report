#include "parser.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QTime>

Parser::Parser ()
{
    work = true;
}

QString JsonToString (QJsonObject *a)
{
    return QString (QJsonDocument (*a).toJson (QJsonDocument::Compact));
}

QJsonObject StringToJson (QString a)
{
    return QJsonDocument::fromJson (a.toUtf8()).object();
}

void Parser::buildTree()
{
    QString dir = str;
    QJsonObject obj;
    obj["dir"] = dir;
    obj["reply"] = "tree";
    if (!QFileInfo (dir).isDir()) {
        emit getResult (obj);
    }
    QJsonArray dirs, files;
    auto list = QDir (dir).entryList (
                    QDir::NoDotAndDotDot | QDir::Dirs);
    for (QFileInfo x : list) {
        dirs.append (x.fileName());
    }
    list = QDir (dir).entryList (
               QDir::NoDotAndDotDot | QDir::Files);
    for (QFileInfo x : list) {
        files.append (x.fileName());
    }

    obj["files"] = files;
    obj["dirs"] = dirs;
    emit getResult (obj);
}


void Parser::slotStopWork (int id)
{
    if (num == id)
        work = false;
};

void Parser::scanner()
{
    QFileInfo f (str);
    if (!f.isFile())
        return;
    QTime dieTime = QTime::currentTime().addSecs (period);
    while (work) {
        if (QTime::currentTime() < dieTime)
            continue;
        dieTime = dieTime.addSecs (period);
        QJsonObject obj;
        QFileInfo f (str);
        obj["file"] = str;
        obj["reply"] = "scan";
        obj["size"] = f.size();
        emit getResult (obj);
    }
    emit finished();
}

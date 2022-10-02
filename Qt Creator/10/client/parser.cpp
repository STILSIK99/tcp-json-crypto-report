#include "parser.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QTime>
#include <QStorageInfo>
#include <QDebug>


using std::string;
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

void Parser::getVolumesList()
{
    QJsonObject obj;
    QJsonArray drives;
    QString vol_name = "A:/";
    for (int i = 1; i < 27; ++i) {
        QStorageInfo storage (vol_name);
        if (storage.isValid()) {
            drives.append (vol_name);
        }
        vol_name = vol_name.replace (0, 1, 'A' + i);
    }
    obj["result"] = drives;
    obj["reply"] = "scan";
    emit getResult (obj);
}


void Parser::thread()
{
    work = true;
    QTime dieTime = QTime::currentTime();
    while (work) {
        if (QTime::currentTime() < dieTime)
            continue;
        dieTime = dieTime.addSecs (period);
        getVolumeSize();
    }
}

void Parser::getVolumeSize()
{
    QStorageInfo storage (vol);
    if (!storage.isValid()) {
        work = false;
        return;
    }
    QJsonObject obj;
    obj["reply"] = "volume";
    obj["volume"] = vol;
    obj["size"] = storage.bytesTotal() / 1024 / 1024;
    obj["free"] = storage.bytesAvailable() / 1024 / 1024;
    emit getResult (obj);
}

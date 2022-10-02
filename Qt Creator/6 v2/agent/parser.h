#ifndef PARSER_H
#define PARSER_H
#include <QJsonObject>
#include <QJsonDocument>
#include <QMap>
#include <mainwindow.h>

struct Request {
    QString action;
    QString table;
    QString data;

    Request() {};

    Request (QString a, QString t, QString d): action (a), table (t), data (d) {};

    QString createMessage ()
    {
        QJsonObject *obj = new QJsonObject;
        (*obj)["action"] = action;
        (*obj)["table"] = table;
        (*obj)["data"] = data;
        return JsonToString (obj);
    }

    Request (QString str)
    {
        QJsonObject obj = StringToJson (str);
        if (obj.contains ("action")) action = obj["action"].toString();
        else action = "";
        if (obj.contains ("table")) table = obj["table"].toString();
        else table = "";
        if (obj.contains ("data")) data = obj["data"].toString();
        else data = "";
    }

    QString JsonToString (QJsonObject *a)
    {
        return QString (QJsonDocument (*a).toJson (QJsonDocument::Compact));
    }

    QJsonObject StringToJson (QString a)
    {
        return QJsonDocument::fromJson (a.toUtf8()).object();
    }
};

#endif // PARSER_H

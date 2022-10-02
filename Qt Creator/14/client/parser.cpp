#include "mainwindow.h"
#include "parser.h"

Parser::Parser (QString dir, QString reg): dir (dir), reg (reg) {}

QString MainWindow::JsonToString (QJsonObject *a)
{
    return QString (QJsonDocument (*a).toJson (QJsonDocument::Compact));
}

QJsonObject MainWindow::StringToJson (QString a)
{
    return QJsonDocument::fromJson (a.toUtf8()).object();
}

void *Parser::scanDir ()
{
    QJsonObject *obj = new QJsonObject;
    (*obj)["dir"] = dir;
    (*obj)["reg"] = reg;
    if (!QFileInfo (dir).isDir()) {
        return obj;
    }
    QRegExp *rx = new QRegExp (reg);
    QJsonArray arr;
    auto list = QDir (dir).entryList (
                    QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::Files);
    for (QFileInfo x : list) {
        qDebug() << x.fileName() << " " << x.fileName().right (4) << "\n";
        QString f;
        auto type = x.fileName().right (4);
        if (type == ".log" || type == ".txt") {
            auto obj = parseLogFile (dir + "\\" + x.fileName(), rx);
            obj["file"] = x.fileName();
            arr.append (obj);
        }
    }
    obj->insert ("results", arr);
    emit getResult (obj);
    qDebug() << "parser " << dir << " finished";
}

QJsonObject Parser::parseLogFile (QString filepath, QRegExp *rx)
{
    QJsonObject res;
    QJsonArray lines;
    QFile file (filepath);
    int counter = 0;
    if (!file.open (QIODevice::ReadOnly | QIODevice::Text)) {
        res["count"] = counter;
        return res;
    }

    while (!file.atEnd()) {
        QString line = file.readLine();
        qDebug() << line;
        if (rx->exactMatch (line)) {
            lines.append (line);
            ++counter;
        }
    }
    file.close();
    res["lines"] = lines;
    res["count"] = counter;
    return res;
}

#ifndef PARSER_H
#define PARSER_H
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QJsonDocument>
#include <windows.h>
#include <psapi.h>
#include <wchar.h>
#include <stdio.h>
#include <winsvc.h>
#include <QObject>
#include <QDirIterator>
#include <QDateTime>
#include <QFileInfo>

class Parser : public QObject
{
        Q_OBJECT

    public:
        Parser ();
        QString str;
        QString b;


    public slots:
        void slotGetSystemInfo();


    signals:
        void getResult (QJsonObject);

};

#endif // PARSER_H

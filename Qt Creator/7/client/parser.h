#ifndef PARSER_H
#define PARSER_H
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QJsonDocument>
#include <windows.h>
#include <tchar.h>
#include <QObject>


class Parser : public QObject
{
        Q_OBJECT

    public:
        Parser ();


    public slots:
        void slotGetSystemInfo();


    signals:
        void getResult (QJsonObject *);

};

#endif // PARSER_H

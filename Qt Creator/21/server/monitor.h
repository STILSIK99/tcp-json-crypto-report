#ifndef MONITOR_H
#define MONITOR_H

#include <QDateTime>
#include <QJsonObject>
#include <QObject>
#include <QPair>
#include <QString>


class Monitor : public QObject
{
        Q_OBJECT

    public:
        Monitor ();
        int period;
        bool work;
        QPair<QString, int> agent;

    public slots:
        void getter();


    signals:
        void requestPacks (QJsonObject, QString, int);
};

#endif // MONITOR_H

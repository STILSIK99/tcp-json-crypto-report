#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QMap>
#include <QString>
#include <QStatusBar>
#include <QPair>

class statBar
{
        QMap<QString, QString> data;
        QStatusBar *stat;
    public:
        void add (QString key, QString value)
        {
            data[key] = value;
        }
        void del (QString key)
        {
            data.remove (key);
        }
        void show()
        {
            QString text = "";
            for (auto x : data) {
                text += x + " - " + data[x] + " | ";
            }
            stat->clearMessage();
            stat->showMessage (text.left (text.length() - 3));
        }
};

#endif

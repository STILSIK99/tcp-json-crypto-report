#ifndef PARSER_H
#define PARSER_H
#include <QObject>
#include <QDebug>
#include <QJsonObject>
#include <QFileInfo>
#include <QRegExp>
#include <QDir>
#include <QFileInfo>
#include <QString>

class Parser : public QObject
{
        Q_OBJECT

    public:
        Parser ();
        bool work;
        int period;
        QString vol;

    private:
        //parser

    public slots:
        void getVolumeSize();
        void thread();
        void getVolumesList();

    signals:
        void getResult (QJsonObject);
};

#endif // PARSER_H

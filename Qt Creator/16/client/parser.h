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
        QString str;
        QString path;
        int num;
    private:
        //parser

    public slots:
        void buildTree();
        void slotStopWork (int);
        void readFile ();

    signals:
        void getResult (QJsonObject *);
        void finished();
};

#endif // PARSER_H

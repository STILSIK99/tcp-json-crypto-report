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
        Parser (QString dir, QString reg);

    private:
        QString dir;
        QString reg;
        //parser
        QJsonObject parseLogFile (QString, QRegExp *);


    public slots:
        void *scanDir ();


    signals:
        void getResult (QJsonObject *);

};

#endif // PARSER_H

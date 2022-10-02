#include "mainwindow.h"
#include "parser.h"

TCHAR *envVarStrings[] = {
    TEXT ("%OS%"),
    TEXT ("%PATH%"),
    TEXT ("%HOMEPATH%"),
    TEXT ("%TEMP%")
};
QVector<QString> envs{
    "OS", "PATH", "HOMEPATH", "TEMP"
};
#define  ENV_VAR_STRING_COUNT  (sizeof(envVarStrings)/sizeof(TCHAR*))
#define INFO_BUFFER_SIZE 32767

Parser::Parser () {}

QString MainWindow::JsonToString (QJsonObject *a)
{
    return QString (QJsonDocument (*a).toJson (QJsonDocument::Compact));
}

QJsonObject MainWindow::StringToJson (QString a)
{
    return QJsonDocument::fromJson (a.toUtf8()).object();
}

void Parser::slotGetSystemInfo ()
{
    QJsonObject *obj = new QJsonObject;
    DWORD i;
    wchar_t infoBuf[INFO_BUFFER_SIZE];
    DWORD  bufCharCount = INFO_BUFFER_SIZE;
    bufCharCount = INFO_BUFFER_SIZE;

    if (GetComputerName ( infoBuf, &bufCharCount ) )
        (*obj)["Computer name"] = QString::fromWCharArray (infoBuf);
    bufCharCount = INFO_BUFFER_SIZE;
    if (GetUserName ( infoBuf, &bufCharCount ) )
        (*obj)["User name"] = QString::fromWCharArray (infoBuf);
    if (GetSystemDirectory ( infoBuf, INFO_BUFFER_SIZE ) )
        (*obj)["System Directory"] = QString::fromWCharArray (infoBuf).replace('\\', '/').replace('//','/');
    if (GetWindowsDirectory ( infoBuf, INFO_BUFFER_SIZE ) )
        (*obj)["Windows Directory"] = QString::fromWCharArray (infoBuf).replace('\\', '/').replace('//','/');
    for ( i = 0; i < ENV_VAR_STRING_COUNT; ++i ) {
        bufCharCount = ExpandEnvironmentStrings (envVarStrings[i], infoBuf,
                       INFO_BUFFER_SIZE );
        if ( bufCharCount <= INFO_BUFFER_SIZE && bufCharCount ){
            QString str = QString::fromWCharArray (infoBuf);
            (*obj)[envs[i]] = str.left(bufCharCount).replace('\\', '/').replace('//','/');
        }
    }
    emit getResult (obj);
}


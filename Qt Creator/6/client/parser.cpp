#include "parser.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QTime>
#include <QStorageInfo>
#include <QDebug>
#include <windows.h>
#include <stdio.h>
#pragma comment(lib, "user32.lib")

typedef unsigned int ui;
typedef long long ll;

using std::string;
Parser::Parser ()
{
    work = true;
}

QString JsonToString (QJsonObject *a)
{
    return QString (QJsonDocument (*a).toJson (QJsonDocument::Compact));
}

QJsonObject StringToJson (QString a)
{
    return QJsonDocument::fromJson (a.toUtf8()).object();
}

void Parser::getSysInfo(){
    SYSTEM_INFO siSysInfo;
    GetSystemInfo(&siSysInfo);
    QJsonObject *obj = new QJsonObject;
    QJsonArray *sys = new QJsonArray;
    (*obj)["reply"] = "info";
    sys->append((ll)ui(siSysInfo.dwOemId));
    sys->append((ll)ui(siSysInfo.dwNumberOfProcessors));
    sys->append((ll)ui(siSysInfo.dwPageSize));
    sys->append((ll)ui(siSysInfo.dwProcessorType));
    sys->append((ll)ui(siSysInfo.lpMinimumApplicationAddress));
    sys->append((ll)ui(siSysInfo.lpMaximumApplicationAddress));
    sys->append((ll)ui(siSysInfo.dwActiveProcessorMask));
    (*obj)["hardware"] = *sys;
    emit getResult(obj);
};

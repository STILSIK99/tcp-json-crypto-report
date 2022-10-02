#include "monitor.h"


void Monitor::getter()
{
    work = true;
    period = 5;
    QDateTime dieTime = QDateTime::currentDateTime().addSecs (period);
    while (work) {
        if (QDateTime::currentDateTime() < dieTime)
            continue;
        dieTime = QDateTime::currentDateTime().addSecs (period);
        QJsonObject obj;
        obj["request"] = "info";
        emit requestPacks (obj, agent.first, agent.second);
    }
}

Monitor::Monitor () {}

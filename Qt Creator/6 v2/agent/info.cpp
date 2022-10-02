#include "info.h"

QJsonObject *GInfo()
{
    QJsonObject *sys = new QJsonObject;
    SYSTEM_INFO siSysInfo;
    // Copy the hardware information to the SYSTEM_INFO structure.
    GetSystemInfo (&siSysInfo);
    char *a = new char[10];
    sprintf (a, "%lx", siSysInfo.lpMinimumApplicationAddress);
    QString lp_min (a);
    sprintf (a, "%lx", siSysInfo.lpMaximumApplicationAddress);
    QString lp_max (a);
    delete []a;
    // Display the contents of the SYSTEM_INFO structure.
    (*sys)["par1"] = int (siSysInfo.dwOemId);
    (*sys)["par2"] = int (siSysInfo.dwNumberOfProcessors);
    (*sys)["par3"] = int (siSysInfo.dwPageSize);
    (*sys)["par4"] = int (siSysInfo.dwProcessorType);
    (*sys)["par5"] = lp_min;
    (*sys)["par6"] = lp_max;
    (*sys)["par7"] = int (siSysInfo.dwActiveProcessorMask);
    return sys;
};

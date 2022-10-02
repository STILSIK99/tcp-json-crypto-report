#ifndef UNICODE
    #define UNICODE
#endif
#include "parser.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QTime>

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "iphlpapi.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

typedef unsigned int ui;
typedef long long ll;

void Parser::getInfo()
{
    QJsonObject obj, reply;
    reply["reply"] = "info";
    DWORD dwRetval;
    MIB_IPSTATS *pStats;
    pStats = (MIB_IPSTATS *) MALLOC (sizeof (MIB_IPSTATS));
    if (pStats == NULL) {
        return;
    }
    dwRetval = GetIpStatistics (pStats);
    if (dwRetval != NO_ERROR)
        return;
    obj["Number of received datagrams"] =  (ll) (ui)pStats->dwInReceives;
    obj["Number of datagrams forwarded"] =  (ll)pStats->dwForwDatagrams;
    obj["Number of outgoing datagrams requested to transmit"] =  (ll) (ui)pStats->dwOutRequests;
    if (pStats)
        FREE (pStats);
    reply["packs"] = obj;
    emit getResult (reply);
}

void Parser::analiticFunction()
{

};

void Parser::getFullInfo()
{
    QJsonObject obj, reply;
    reply["reply"] = "report";

    DWORD dwRetval;
    MIB_IPSTATS *pStats;

    pStats = (MIB_IPSTATS *) MALLOC (sizeof (MIB_IPSTATS));

    if (pStats == NULL) {
        return;
    }
    dwRetval = GetIpStatistics (pStats);
    if (dwRetval != NO_ERROR)
        return;
    switch (pStats->dwForwarding) {
        case MIB_IP_FORWARDING:
            obj["IP forwarding"] = "Enabled";
            break;
        case MIB_IP_NOT_FORWARDING:
            obj["IP forwarding"] = "Disabled";
            break;
        default:
            obj["IP forwarding"] = "Unknown value";
            break;
    }

    obj["Default initial TTL"] = (ll) (ui)pStats->dwDefaultTTL;
    obj["Number of received datagrams"] =  (ll) (ui)pStats->dwInReceives;
    obj["Number of received datagrams with header errors"] =  (ll) (ui)pStats->dwInHdrErrors;
    obj["Number of received datagrams with address errors"] =  (ll) (ui)pStats->dwInAddrErrors;

    obj["Number of datagrams forwarded"] =  (ll)pStats->dwForwDatagrams;

    obj["Number of received datagrams with an unknown protocol"] =  (ll) (ui)pStats->dwInUnknownProtos;
    obj["Number of received datagrams discarded"] =  (ll) (ui)pStats->dwInDiscards;
    obj["Number of received datagrams delivered"] =  (ll) (ui)pStats->dwInDelivers;

    obj["Number of outgoing datagrams requested to transmit"] =  (ll) (ui)pStats->dwOutRequests;
    obj["Number of outgoing datagrams discarded for routing"] =  (ll) (ui)pStats->dwRoutingDiscards;
    obj["Number of outgoing datagrams discarded"] =  (ll) (ui)pStats->dwOutDiscards;
    obj["Number of outgoing datagrams with no route to destination discarded"] =  (ll) (ui)pStats->dwOutNoRoutes;

    obj["Fragment reassembly timeout"] =  (ll) (ui)pStats->dwReasmTimeout;
    obj["Number of datagrams that required reassembly"] =  (ll) (ui)pStats->dwReasmReqds;
    obj["Number of datagrams successfully reassembled"] =  (ll) (ui)pStats->dwReasmOks;
    obj["Number of datagrams that could not be reassembled"] =  (ll) (ui)pStats->dwReasmFails;

    obj["Number of datagrams fragmented successfully"] =  (ll) (ui)pStats->dwFragOks;
    obj["Number of datagrams not fragmented and discarded"] =  (ll) (ui)pStats->dwFragFails;
    obj["Number of fragments created"] =  (ll) (ui)pStats->dwFragCreates;

    obj["Number of interfaces"] =  (ll) (ui)pStats->dwNumIf;
    obj["Number of IP addresses"] =  (ll) (ui)pStats->dwNumAddr;
    obj["Number of routes"] =  (ll) (ui)pStats->dwNumRoutes;

    if (pStats)
        FREE (pStats);

    reply["report"] = obj;
    emit getResult (reply);
}

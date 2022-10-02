#include "agent.h"
#include <QApplication>

int main (int argc, char *argv[])
{
    QApplication a (argc, argv);
    Agent ag;
    return a.exec();
}

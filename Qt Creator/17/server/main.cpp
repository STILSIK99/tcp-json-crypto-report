#include "maneger.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Maneger w;
    w.show();
    return a.exec();
}

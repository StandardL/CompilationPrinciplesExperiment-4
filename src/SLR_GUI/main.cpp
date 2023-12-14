#include "SLR_GUI.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SLR_GUI w;
    w.show();
    return a.exec();
}

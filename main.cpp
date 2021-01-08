#include "FxMainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FxMainWindow w;

    w.show();
    return a.exec();
}

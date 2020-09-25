#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    mainwindow win;
    win.show();

    return a.exec();
}
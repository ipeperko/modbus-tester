#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <modbus/modbus.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << "modbus_tester - libmodbus version" << LIBMODBUS_VERSION_STRING;

    mainwindow win;
    win.show();

    return a.exec();
}
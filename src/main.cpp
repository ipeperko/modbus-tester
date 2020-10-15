#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include "mb_tester_common.h"
#include <modbus/modbus.h>

int main(int argc, char *argv[])
{
    qRegisterMetaType<QVector<mb_register_t>>("QVector<mb_register_t>");
    qRegisterMetaType<QVector<mb_input_t>>("QVector<mb_input_t>");
    qRegisterMetaTypeStreamOperators<QVector<mb_register_t>>("QVector<mb_register_t>");
    qRegisterMetaTypeStreamOperators<QVector<mb_input_t>>("QVector<mb_input_t>");

    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("modbus_tester");

    qDebug() << "modbus_tester - libmodbus version" << LIBMODBUS_VERSION_STRING;

    mainwindow win;
    win.show();

    return a.exec();
}
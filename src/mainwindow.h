#ifndef MODBUS_TESTER_MAINWINDOW_H
#define MODBUS_TESTER_MAINWINDOW_H

#include "client_session.h"
#include <QMainWindow>
#include <memory>

class QTableWidgetItem;

namespace Ui {
class MainWindow;
class ClientForm;
}

class IPv4_validator;
class client_session;

class mainwindow : public QMainWindow
{
    Q_OBJECT
public:
    mainwindow(QWidget* parent = nullptr);
    ~mainwindow() override;

private:
    Ui::MainWindow* ui {nullptr};
};


#endif //MODBUS_TESTER_MAINWINDOW_H

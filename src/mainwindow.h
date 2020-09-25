#ifndef MODBUS_TESTER_MAINWINDOW_H
#define MODBUS_TESTER_MAINWINDOW_H

#include "client_session.h"
#include <QMainWindow>
#include <memory>

class QTableWidgetItem;

namespace Ui {
class MainWindow;
}

class client_tab;
class server_tab;

class mainwindow : public QMainWindow
{
    Q_OBJECT
public:
    mainwindow(QWidget* parent = nullptr);
    ~mainwindow() override;

private:
    void update_tab_status(int index, bool active);

    Ui::MainWindow* ui {nullptr};
    client_tab* client;
    server_tab* server;
};


#endif //MODBUS_TESTER_MAINWINDOW_H

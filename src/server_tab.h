#ifndef MODBUS_TESTER_SERVER_TAB_H
#define MODBUS_TESTER_SERVER_TAB_H

#include <QWidget>
#include "server_session.h"

namespace Ui {
class ServerForm;
}

class server_reg_model;

class server_tab : public QWidget
{
    Q_OBJECT
public:
    explicit server_tab(QWidget* parent = nullptr);
    ~server_tab() override;

private slots:
    void connection_type_changed();
    void connect_clicked();

private:
    void append_log_msg(const QString& msg);
    void append_log_error(const QString& msg);

    Ui::ServerForm* ui {nullptr};
    std::unique_ptr<server_session> server;
    server_reg_model* reg_model;

signals:
    void connection_status_changed(bool active);
};


#endif //MODBUS_TESTER_SERVER_TAB_H

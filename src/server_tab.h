#ifndef MODBUS_TESTER_SERVER_TAB_H
#define MODBUS_TESTER_SERVER_TAB_H

#include <QWidget>
#include "server_session.h"

namespace Ui {
class ServerForm;
}

class server_tab : public QWidget
{
    Q_OBJECT
public:
    server_tab(QWidget* parent = nullptr);
    ~server_tab() override;

//    modbus_mapping_t data;

private slots:
    void connection_type_changed();
    void connect_clicked();

private:
    void append_log_msg(const QString& msg);
    void append_log_error(const QString& msg);

    Ui::ServerForm* ui {nullptr};
    std::unique_ptr<server_session> server;
};


#endif //MODBUS_TESTER_SERVER_TAB_H

#ifndef MODBUS_TESTER_CLIENT_TAB_H
#define MODBUS_TESTER_CLIENT_TAB_H

#include "client_session.h"
#include <QMainWindow>
#include <QListWidgetItem>
#include <memory>

class QTableWidgetItem;

namespace Ui {
class ClientForm;
}

class IPv4_validator;
class client_session;
class log_widget_item;
class rtu_widget;

class client_tab : public QWidget
{
    Q_OBJECT

    class logger_helper;
    friend class logger_helper;
public:
    static const char* data_index_str(mb_dropdown_data_index_t type);

    client_tab(QWidget* parent = nullptr);
    ~client_tab() override;

private slots:
    void connection_type_changed();
    void connect_clicked();
    void data_type_changed();
    void data_address_changed();
    void init_table(int addr, int size);
    void populate_table(int addr, mb_reg_vector const& data);
    void populate_table(int addr, mb_bit_vector const& data);
    void clear_table_values();
    void validate_table_cell(QTableWidgetItem* item);
    void data_read();
    void data_write();
    void check_socket();
    void log_widget_action(log_widget_item const* item);
    void append_log_msg(const QString& msg);
    void append_log_error(const QString& msg);

private:
    rtu_widget* rtu_widget_{nullptr};
    IPv4_validator* ip4_validator{nullptr};
    Ui::ClientForm* ui{nullptr};
    std::unique_ptr<client_session> client_;

signals:
    void connection_status_changed(bool active);
};

#endif //MODBUS_TESTER_CLIENT_TAB_H

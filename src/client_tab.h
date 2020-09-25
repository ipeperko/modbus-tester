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

class client_tab : public QWidget
{
    Q_OBJECT
public:
    enum class mb_data_index_t : int
    {
        holding_registers = 0,
        input_registers,
        coils,
        discrete_inputs
    };

    static const char* data_index_str(mb_data_index_t type);

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
    void append_log(bool write, mb_data_index_t cmd, int addr, int size);
    void append_log_msg(const QString& msg);
    void append_log_error(const QString& msg);

private:
    IPv4_validator* ip4_validator{nullptr};
    Ui::ClientForm* ui{nullptr};
    std::unique_ptr<client_session> client_;
};

class client_log_widget_item : public QListWidgetItem
{
public:
    client_log_widget_item(QListWidget *parent, client_tab::mb_data_index_t role, int addr, int num)
        : QListWidgetItem(parent)
        , role(role)
        , addr(addr)
        , num(num)
    {}

    client_tab::mb_data_index_t role;
    int addr;
    int num;
};

#endif //MODBUS_TESTER_CLIENT_TAB_H

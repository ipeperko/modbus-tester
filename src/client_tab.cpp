#include "client_tab.h"
#include "IPv4_validator.h"
#include "client_session_tcp.h"
#include "ui_client_form.h"
#include <QDateTime>
#include <QDebug>

const char* client_tab::data_index_str(mb_data_index_t type)
{
    switch (type) {
        case mb_data_index_t::holding_registers:
            return "holding registers";
        case mb_data_index_t::input_registers:
            return "input registers";
        case mb_data_index_t::coils:
            return "coils";
        case mb_data_index_t::discrete_inputs:
            return "discrete inputs";
        default:
            return "";
    }
}

client_tab::client_tab(QWidget *parent)
    : QWidget(parent)
    , ip4_validator(new IPv4_validator(this))
    , ui(new Ui::ClientForm)
{
    ui->setupUi(this);

    ui->lineEdit_ipAddress->setValidator(ip4_validator);
    ui->radioButton_TCP->setChecked(true);
    ui->groupBox_Communication->setEnabled(false);

    connect(ui->radioButton_TCP, &QRadioButton::clicked, this, &client_tab::connection_type_changed);
    connect(ui->radioButton_RTU, &QRadioButton::clicked, this, &client_tab::connection_type_changed);
    connect(ui->pushButton_Connect, &QPushButton::clicked, this, &client_tab::connect_clicked);
    //connect(ui_client->pushButton_CheckSocket, &QPushButton::clicked, this, &client_tab::check_socket);
    connect(ui->comboBox_DataType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &client_tab::data_type_changed);
    connect(ui->spinBox_DataAddress, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
            &client_tab::data_address_changed);
    connect(ui->spinBox_DataSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
            &client_tab::data_address_changed);
    connect(ui->pushButton_ReadData, &QPushButton::clicked, this, &client_tab::data_read);
    connect(ui->pushButton_WriteData, &QPushButton::clicked, this, &client_tab::data_write);
    connect(ui->tableWidget, &QTableWidget::itemChanged, this, &client_tab::validate_table_cell);

    data_address_changed();
    connection_type_changed();
    data_type_changed();
    clear_table_values();
}

client_tab::~client_tab()
{
    delete ui;
}

void client_tab::connection_type_changed()
{
    bool is_tcp = ui->radioButton_TCP->isChecked();

    ui->frame_TCP->setVisible(is_tcp);
    ui->frame_RTU->setVisible(!is_tcp);
}

void client_tab::connect_clicked()
{
    bool is_tcp = ui->radioButton_TCP->isChecked();
    bool connected = client_.get() != nullptr;

    if (connected) {
        client_.reset();
        append_log_msg("disconnected");
    }
    else {
        try {
            append_log_msg("connecting ...");
            if (is_tcp) {
                client_ = std::make_unique<client_session_tcp>(ui->lineEdit_ipAddress->text().toStdString(),
                                                               ui->spinBox_TCPPort->value());
            }
            else {
                throw std::runtime_error("TODO: RTU client session");
            }

            client_->connect();
            client_->set_slave_address(ui->spinBox_ClientSlaveAddress->value());
            client_->set_debug(ui->checkBox_Debug->isChecked());
            append_log_msg("connected");
        }
        catch (std::exception& e) {
            client_.reset();
            append_log_error("Client connect failed - " + QString(e.what()));
        }
    }

    connected = client_ != nullptr;

    ui->radioButton_TCP->setEnabled(!connected);
    ui->radioButton_RTU->setEnabled(!connected);
    ui->frame_TCP->setEnabled(!connected);
    ui->frame_RTU->setEnabled(!connected);
    ui->frame_ConnectionCommon->setEnabled(!connected);
    ui->pushButton_Connect->setText(connected ? "Disconnect" : "Connect");

    ui->groupBox_Communication->setEnabled(connected);
}

void client_tab::data_type_changed()
{
    auto type = static_cast<mb_data_index_t>(ui->comboBox_DataType->currentIndex());
    bool read_only = type == mb_data_index_t::input_registers || type == mb_data_index_t::discrete_inputs;

    ui->pushButton_WriteData->setEnabled(!read_only);
}

void client_tab::data_address_changed()
{
    int addr = ui->spinBox_DataAddress->value();
    int n = ui->spinBox_DataSize->value();

    init_table(addr, n);
}

void client_tab::init_table(int addr, int size)
{
    ui->tableWidget->setColumnCount(size);
    QStringList header;
    for (int i = 0; i < size; i++) {
        header.push_back(QString::number(addr + i));
        if (!ui->tableWidget->item(0, i)) {
            auto item = new QTableWidgetItem;
            item->setTextAlignment(Qt::AlignHCenter);
            ui->tableWidget->setItem(0, i, item);
        }
    }
    ui->tableWidget->setHorizontalHeaderLabels(header);
}

void client_tab::populate_table(int addr, mb_reg_vector const& data)
{
    init_table(addr, data.size());

    for (int i = 0; i < data.size(); i++) {
        ui->tableWidget->item(0, i)->setText(QString::number(data[i]));
    }
}

void client_tab::populate_table(int addr, mb_bit_vector const& data)
{
    init_table(addr, data.size());

    for (int i = 0; i < data.size(); i++) {
        ui->tableWidget->item(0, i)->setText(QString::number(data[i]));
    }
}

void client_tab::clear_table_values()
{
    for (int i = 0; i < ui->tableWidget->columnCount(); i++) {
        ui->tableWidget->item(0, i)->setText("");
    }
}

void client_tab::validate_table_cell(QTableWidgetItem* item)
{
    if (!item || item->text().isEmpty())
        return;

    auto type = static_cast<mb_data_index_t>(ui->comboBox_DataType->currentIndex());
    bool ok;
    int val = item->text().toInt(&ok);

    switch (type) {
        case mb_data_index_t::coils:
        case mb_data_index_t::discrete_inputs:
            if (!ok || (val != 0 && val != 1)) {
                item->setText("0");
            }
            break;
        case mb_data_index_t::holding_registers:
        case mb_data_index_t::input_registers:
            if (!ok || (val < std::numeric_limits<int16_t>::min()) || (val > std::numeric_limits<uint16_t>::max())) {
                item->setText("0");
            }
            break;
        default:;
    }
}

void client_tab::data_read()
{
    if (!client_) {
        qWarning() << "Client is null - cannot read";
        return;
    }

    try {
        auto type = static_cast<mb_data_index_t>(ui->comboBox_DataType->currentIndex());
        int addr = ui->spinBox_DataAddress->value();
        int n = ui->spinBox_DataSize->value();

        if (n < 1) {
            append_log_error("Data size must be greater than 0");
            return;
        }

        append_log(false, type, addr, n);

        switch (type) {
            case mb_data_index_t::holding_registers:
            {
                auto res = client_->read_holding_registers(addr, n);
                populate_table(addr, res);
            }
                break;
            case mb_data_index_t::input_registers:
            {
                auto res = client_->read_input_registers(addr, n);
                populate_table(addr, res);
            }
                break;
            case mb_data_index_t::coils:
            {
                auto res = client_->read_coils(addr, n);
                populate_table(addr, res);
            }
                break;
            case mb_data_index_t::discrete_inputs:
            {
                auto res = client_->read_discrete_inputs(addr, n);
                populate_table(addr, res);
            }
                break;
            default:;
        }
    }
    catch (std::exception& e) {
        clear_table_values();
        append_log_error("Error " + QString(e.what()));
    }
}

void client_tab::data_write()
{
    if (!client_) {
        qWarning() << "Client is null - cannot write";
        return;
    }

    try {
        auto type = static_cast<mb_data_index_t>(ui->comboBox_DataType->currentIndex());
        int addr = ui->spinBox_DataAddress->value();
        int n = ui->spinBox_DataSize->value();

        if (n < 1) {
            append_log_error("Data size must be greater than 0");
            return;
        }

        append_log(true, type, addr, n);

        switch (type) {
            case mb_data_index_t::holding_registers:
            {
                mb_reg_vector data;

                for (int i = 0; i < n; i++) {
                    bool ok;
                    int val = ui->tableWidget->item(0, i)->text().toInt(&ok);
                    if (!ok) {
                        val = 0;
                        ui->tableWidget->item(0, i)->setText("0");
                    }
                    data.push_back(static_cast<uint16_t>(val));
                }

                client_->write_holding_registers(addr, data);
            }
                break;
            case mb_data_index_t::coils:
            {
                mb_bit_vector data;

                for (int i = 0; i < n; i++) {
                    bool ok;
                    int val = ui->tableWidget->item(0, i)->text().toInt(&ok);
                    if (!ok || (val != 0 && val != 1)) {
                        val = 0;
                        ui->tableWidget->item(0, i)->setText("0");
                    }
                    data.push_back(val);
                }

                client_->write_coils(addr, data);
            }
                break;
            default:;
        }
    }
    catch (std::exception& e) {
        append_log_error("Error " + QString(e.what()));
    }
}

void client_tab::check_socket()
{
    if (client_) {
        bool valid = client_->check_socket();
        if (valid) {
            qDebug() << "Client socket is valid";
        }
        else {
            qWarning() << "Client socket not valid";
        }
    }
}

void client_tab::append_log(bool write, mb_data_index_t cmd, int addr, int size)
{
    QString s = write ? "WRITE " : "READ  ";
    s += "[";
    s += data_index_str(cmd);
    s += "]  address " + QString::number(addr) + "  size " + QString::number(size);

    auto* item = new client_log_widget_item(nullptr, cmd, addr, size);
    ui->listWidget->append_item(item, s);
}

void client_tab::append_log_msg(const QString &msg)
{
    ui->listWidget->append_log_message(msg);
}

void client_tab::append_log_error(const QString &msg)
{
    ui->listWidget->append_log_error(msg);
}

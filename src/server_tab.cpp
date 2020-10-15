#include "server_tab.h"
#include "server_reg_model.h"
#include "ui_server_form.h"
#include <QDebug>

server_tab::server_tab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServerForm)
{
    ui->setupUi(this);
    ui->radioButton_TCP->setChecked(true);

    reg_model = new server_reg_model(this);
    ui->tableView_Registers->setModel(reg_model);

    for (int i = 0; i < reg_model->columnCount({}); i++) {
        ui->tableView_Registers->setColumnWidth(i, 80);
    }

    connect(ui->radioButton_TCP, &QRadioButton::clicked, this, &server_tab::connection_type_changed);
    connect(ui->radioButton_RTU, &QRadioButton::clicked, this, &server_tab::connection_type_changed);
    connect(ui->pushButton_Connect, &QPushButton::clicked, this, &server_tab::connect_clicked);

    connection_type_changed();
    ui->pushButton_Connect->update_status(false);
}

server_tab::~server_tab()
{
    delete ui;
}

void server_tab::connection_type_changed()
{
    bool is_tcp = ui->radioButton_TCP->isChecked();

    ui->frame_TCP->setVisible(is_tcp);
    ui->frame_RTU->setVisible(!is_tcp);
}

void server_tab::connect_clicked()
{
    bool connected = server.get() != nullptr;

    if (connected) {
        append_log_msg("Disconnecting...");
        server.reset();
        append_log_msg("Disconnected");
    }
    else {
        try {
            append_log_msg("Connecting...");
            server = std::make_unique<server_session_tcp>(ui->spinBox_TCPPort->value(), reg_model->mapping());
            connect(server.get(), &server_session::message, this, &server_tab::append_log_msg);
            connect(server.get(), &server_session::error_message, this, &server_tab::append_log_error);
            server->set_debug(ui->checkBox_Debug->isChecked());
            server->start_server();
            append_log_msg("Connected");
            connect(server.get(), &server_session::data_changed, reg_model, &server_reg_model::refresh_all);
        }
        catch(std::exception& e) {
            append_log_error(e.what());
        }
    }

    connected = server.get() != nullptr;

    ui->pushButton_Connect->update_status(connected);
    emit connection_status_changed(connected);
}

void server_tab::append_log_msg(const QString& msg)
{
    ui->listWidget->append_log_message(msg);
}

void server_tab::append_log_error(const QString& msg)
{
    ui->listWidget->append_log_error(msg);
}
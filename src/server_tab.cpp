#include "server_tab.h"
#include "ui_server_form.h"
#include <QDebug>

server_tab::server_tab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServerForm)
{
    ui->setupUi(this);
    ui->radioButton_TCP->setChecked(true);
    connect(ui->radioButton_TCP, &QRadioButton::clicked, this, &server_tab::connection_type_changed);
    connect(ui->radioButton_RTU, &QRadioButton::clicked, this, &server_tab::connection_type_changed);
    connect(ui->pushButton_Connect, &QPushButton::clicked, this, &server_tab::connect_clicked);

    connection_type_changed();
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
        append_log_msg("disconnecting...");
        server.reset();
        append_log_msg("disconnected");
    }
    else {
        try {
            append_log_msg("connecting...");
            server = std::make_unique<server_session>(ui->spinBox_TCPPort->value());
            connect(&server->message_emitter, &server_task_message_emitter::message, this, &server_tab::append_log_msg);
            connect(&server->message_emitter, &server_task_message_emitter::error_message, this, &server_tab::append_log_error);
            server->set_debug(ui->checkBox_Debug->isChecked());
            server->start_server();
            append_log_msg("connected");
        }
        catch(std::exception& e) {
            append_log_error(e.what());
        }
    }

    connected = server.get() != nullptr;

    ui->pushButton_Connect->setText(connected ? "Disconnect" : "Connect");
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
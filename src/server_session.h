#ifndef MODBUS_TESTER_SERVER_SESSION_H
#define MODBUS_TESTER_SERVER_SESSION_H

#include "session_base.h"
#include <thread>
#include <QObject>

class server_task_message_emitter : public QObject
{
    Q_OBJECT
public:
    server_task_message_emitter(QObject* parent = nullptr)
        : QObject(parent)
    {}

signals:
    void message(const QString& msg);
    void error_message(const QString& msg);
};

class server_session : public session_base
{
public:

    server_session(int port);
    ~server_session() override;

    void start_server();
    void stop_server();

    modbus_mapping_t* mb_map {nullptr};
    server_task_message_emitter message_emitter;

    static constexpr size_t buffer_size_bits = 1024;
    static constexpr size_t buffer_size_coils = 1024;
    static constexpr size_t buffer_size_holding_register = 1024;
    static constexpr size_t buffer_size_input_registers = 1024;

private:
    void task();
    int modbusServerReply(const std::vector<uint8_t>& query);
    void on_task_error(std::string const& what);

    int socket {-1};
    std::thread thr;
    bool do_run {false};


//    uint8_t buff_bits[buffer_size_bits] {};
//    uint8_t buff_coils[buffer_size_coils] {};
//    uint8_t buff_holding_register[buffer_size_holding_register] {};
//    uint8_t buff_input_registers[buffer_size_input_registers] {};

    static constexpr int max_connections = 5;
};

#endif //MODBUS_TESTER_SERVER_SESSION_H

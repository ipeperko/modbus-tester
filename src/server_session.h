#ifndef MODBUS_TESTER_SERVER_SESSION_H
#define MODBUS_TESTER_SERVER_SESSION_H

#include "session_base.h"
#include <thread>

//
// Server session base class
//
class server_session : public session_base
{
    Q_OBJECT
public:
    explicit server_session(modbus_mapping_t& map);
    ~server_session() override;

    void start_server();
    virtual void stop_server();

    modbus_mapping_t& mb_map;

protected:
    virtual void task() {}
    int server_reply(const std::vector<uint8_t>& query);

    std::thread thr;
    std::atomic<bool> do_run {false};

signals:
    void data_changed();
    void message(const QString& msg);
    void error_message(const QString& msg);
};

//
// Server session TCP
//
class server_session_tcp : public server_session
{
public:
    server_session_tcp(int port, modbus_mapping_t& map);

    void stop_server() override;

private:
    void task() override;

    int sock_listen {-1};
    int sock_accept {-1};
};

//
// Server session RTU
//
class server_session_rtu : public server_session
{
public:
    server_session_rtu(const QString& tty, mb_rtu_type type, int baud, char parity, int data_bit, int stop_bit, int rts, modbus_mapping_t& map);

    void stop_server() override;

private:
    void task() override;
};

#endif //MODBUS_TESTER_SERVER_SESSION_H

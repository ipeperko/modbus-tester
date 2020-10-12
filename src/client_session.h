#ifndef MODBUS_TESTER_CLIENT_SESSION_H
#define MODBUS_TESTER_CLIENT_SESSION_H

#include "session_base.h"

//
// Client session interface class
//
class client_session : public session_base
{
    Q_OBJECT
public:
    virtual ~client_session();

    void connect();
    void set_slave_address(int addr);
    void set_response_timeout(unsigned milliseconds);

    mb_reg_vector read_holding_registers(int addr, int size);
    mb_reg_vector read_input_registers(int addr, int size);
    mb_bit_vector read_coils(int addr, int size);
    mb_bit_vector read_discrete_inputs(int addr, int size);

    void write_holding_registers(int addr, mb_reg_vector const& data);
    void write_coils(int addr, mb_bit_vector const& data);
};

//
// Client session TCP
//
class client_session_tcp : public client_session
{
public:
    client_session_tcp(const QString& ip, int port);

};

//
// Client session RTU
//
class client_session_rtu : public client_session
{
public:
    client_session_rtu(const QString& tty, mb_rtu_type type, int baud, char parity, int data_bit, int stop_bit, int rts);
};


#endif //MODBUS_TESTER_CLIENT_SESSION_H

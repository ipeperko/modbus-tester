#ifndef MODBUS_TESTER_SESSION_BASE_H
#define MODBUS_TESTER_SESSION_BASE_H

#include "mb_tester_common.h"
#include <modbus/modbus.h>

class session_base
{
public:
    virtual ~session_base();

    void disconnect();
    bool check_socket();
    void set_debug(bool enable);

protected:
    void on_error(std::string_view what);

    modbus_t *ctx {nullptr};
};

#endif //MODBUS_TESTER_SESSION_BASE_H

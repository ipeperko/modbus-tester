#include "session_base.h"

session_base::~session_base()
{
    if (ctx) {
        disconnect();
        modbus_free(ctx);
        ctx = nullptr;
    }
}

void session_base::disconnect()
{
    modbus_close(ctx);
}

bool session_base::check_socket()
{
    return modbus_get_socket(ctx) >= 0;
}

void session_base::set_debug(bool enable)
{
    int rc = modbus_set_debug(ctx, enable);
    if (rc != 0) {
        on_error("Modbus set debug failed");
    }
}

void session_base::on_error(std::string_view what)
{
    int err = errno;
    throw mb_exception(what.data(), err);
}
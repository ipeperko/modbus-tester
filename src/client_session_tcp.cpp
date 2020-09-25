#include "client_session_tcp.h"

client_session_tcp::client_session_tcp(std::string_view ip, int port)
{
    ctx = modbus_new_tcp(ip.data(), port);
}

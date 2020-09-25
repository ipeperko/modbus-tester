#ifndef MODBUS_TESTER_CLIENT_SESSION_TCP_H
#define MODBUS_TESTER_CLIENT_SESSION_TCP_H

#include "client_session.h"
#include <string_view>

class client_session_tcp : public client_session
{
public:
    client_session_tcp(std::string_view ip, int port);

};


#endif //MODBUS_TESTER_CLIENT_SESSION_TCP_H

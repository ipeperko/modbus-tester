#include "server_session.h"
#include <sys/socket.h>
#include <sstream>
#include <QDebug>
#include <unistd.h>

//
// Server session base class
//
server_session::server_session(modbus_mapping_t& map)
    : session_base()
    , mb_map(map)
{
}

server_session::~server_session()
{
    // Stop server if running
    stop_server();
}

void server_session::start_server()
{
    // Stop server if running
    stop_server();

    // Enable task execution
    do_run = true;

    // Start thread
    thr = std::thread([this]() {
        try {
            task();
        }
        catch (std::exception& e) {
            emit error_message(e.what());
        }
    });
}

void server_session::stop_server()
{
    // Disable task execution
    do_run = false;

    // Join thread
    if (thr.joinable()) {
        thr.join();
    }
}

int server_session::server_reply(const std::vector<uint8_t>& query)
{
    unsigned unitid = query[6];
    unsigned cmd = query[7];
    bool is_write = cmd == MODBUS_FC_WRITE_MULTIPLE_COILS ||
        cmd == MODBUS_FC_WRITE_MULTIPLE_REGISTERS ||
        cmd == MODBUS_FC_WRITE_SINGLE_COIL ||
        cmd == MODBUS_FC_WRITE_SINGLE_REGISTER;

    {
        std::ostringstream os;
        os << "Received cmd " << cmd << " (0x" << std::hex << cmd << std::dec <<
            ") from device id 0x" << std::hex << unitid << std::dec <<
            " (num bytes : " << query.size() << ") :";

        for (auto it : query) {
            os << " " << std::hex << static_cast<int>(it) << std::dec;
        }

        emit message(os.str().c_str());
    }

    int rc = modbus_reply(ctx, &query[0], query.size(), &mb_map);

    if (rc < 0) {
        emit error_message(QString("Modbus sending error - %1").arg(modbus_strerror(errno)));
    }
    else {
        qDebug() << "Replying to request bytes : " << rc;
        if (is_write) {
            emit data_changed();
        }
    }

    return rc;
}

//
// Server session TCP
//
server_session_tcp::server_session_tcp(int port, modbus_mapping_t& map)
    : server_session(map)
{
    // Create modbus context
    ctx = modbus_new_tcp("0.0.0.0", port);

    if (!ctx) {
        on_error_exception("Cannot create new Modbus tcp connection");
    }
}

void server_session_tcp::stop_server()
{
    // Disable task execution
    do_run = false;

    // Shutdown listener socket
    if (sock_listen > -1) {
        shutdown(sock_listen, SHUT_RDWR);
        sock_listen = -1;
    }

    // Shutdown accept socket
    if (sock_accept > -1) {
        shutdown(sock_accept, SHUT_RDWR);
        sock_accept = -1;
    }

    server_session::stop_server();
}


void server_session_tcp::task()
{
    RAII_helper task_debug_msg([&]() {
        qDebug() << "Server task started";
    }, [&]() {
        qDebug() << "Server task finished";
    });

    // Socket listen
    sock_listen = modbus_tcp_listen(ctx, 1);

    if (sock_listen < 0) {
        on_error_exception("Modbus tcp server socket failed");
    }

    while (do_run) {
        // Accept connection
        int s = modbus_tcp_accept(ctx, &sock_listen);

        if (s < 0) {
            if (do_run) {
                // emit error message only if do_run is true
                // otherwise it is not considered as an error
                emit error_message("TCP accept failed");
            }
            std::this_thread::yield();
            continue;
        }

        emit message("Accepted connection");

        // s is a new opened socket and should be closed after receive is completed
        RAII_helper sock_auto_close([&]() {
            sock_accept = s;
        }, [&]() {
            close(sock_accept);
            sock_accept = -1;
        });

        // Receive data
        int nsend;

        do {
            // Receive buffer
            std::vector<uint8_t> query(MODBUS_TCP_MAX_ADU_LENGTH, 0);

            // Receive some data
            int nrcv = modbus_receive(ctx, &query[0]);

            if (nrcv < 0) {
                // Receive error
                qDebug() << "Receive error " << modbus_strerror(errno);
                break;
            }
            else if (nrcv < 10) {
                // Reply exception
                emit error_message(QString("Modbus server received < 10 bytes (%1)").arg(nrcv));
                nsend = modbus_reply_exception(ctx, &query[0], MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
            }
            else {
                // Process request
                query.resize(nrcv);
                nsend = server_reply(query);
            }

        } while(nsend > 0);
    }
}

//
// Server session RTU
//
server_session_rtu::server_session_rtu(const QString &tty, mb_rtu_type type, int baud, char parity, int data_bit, int stop_bit, int rts, int slave_address, double timeout, modbus_mapping_t &map)
    : server_session(map)
{
    ctx = modbus_new_rtu(tty.toStdString().c_str(), baud, parity, data_bit, stop_bit);

    if (!ctx) {
        on_error_exception("Cannot create new Modbus rtu connection");
    }

    modbus_rtu_set_serial_mode(ctx, type == mb_rtu_type::RS232 ? MODBUS_RTU_RS232 : MODBUS_RTU_RS485);
    modbus_rtu_set_rts(ctx, rts);
    modbus_set_slave(ctx, slave_address);
    modbus_set_response_timeout(ctx, static_cast<int>(timeout), (timeout - static_cast<int>(timeout)) * 1000000);
}

void server_session_rtu::stop_server()
{
    server_session::stop_server();
}

void server_session_rtu::task()
{
    RAII_helper task_debug_msg([&]() {
        qDebug() << "Server task started";
    }, [&]() {
        qDebug() << "Server task finished";
    });

    int ret = modbus_connect(ctx);
    if(ret < 0){
        emit error_message("modbus_connect error");
        return;
    }

    while (do_run) {
        // Receive data
        int nsend;

        do {
            // Receive buffer
            std::vector<uint8_t> query(MODBUS_RTU_MAX_ADU_LENGTH, 0);

            // Receive some data
            int nrcv; // = modbus_receive(ctx, &query[0]);
            do {
                nrcv = modbus_receive(ctx, &query[0]);
            } while (nrcv == 0);

            if (nrcv < 0) {
                // Receive error
                qDebug() << "Receive error " << modbus_strerror(errno);
                break;
            }
//            else if (nrcv < 10) {
//                // Reply exception
//                emit error_message(QString("Modbus server received < 10 bytes (%1)").arg(nrcv));
//                nsend = modbus_reply_exception(ctx, &query[0], MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
//            }
            else {
                // Process request
                query.resize(nrcv);
                nsend = server_reply(query);
            }

        } while(nsend > 0);
    }
}

#include "server_session.h"
#include <sys/socket.h>
#include <sstream>
#include <QDebug>
#include <unistd.h>

server_session::server_session(int port)
    : session_base()
{
    mb_map = modbus_mapping_new(buffer_size_coils, buffer_size_coils, buffer_size_holding_register, buffer_size_input_registers);

    ctx = modbus_new_tcp("0.0.0.0", port);
    if (!ctx) {
        on_error_exception("Cannot create new Modbus tcp connection");
    }
}

server_session::~server_session()
{
    stop_server();

    if (mb_map) {
        modbus_mapping_free(mb_map);
    }
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

    // Join thread
    if (thr.joinable()) {
        thr.join();
    }
}

class socket_cleaner
{
public:
    explicit socket_cleaner(int& s) : sock(s) {}
    ~socket_cleaner()
    {
        close(sock);
        sock = -1;
    }
private:
    int& sock;
};

void server_session::task()
{
    qDebug() << "Server task started";

    sock_listen = modbus_tcp_listen(ctx, 1);
    if (sock_listen < 0) {
        on_error_exception("Modbus tcp server socket failed");
    }

    while (do_run) {
        qDebug() << "Server accepting...";
        int s = modbus_tcp_accept(ctx, &sock_listen);
        qDebug() << "Server accepted - code : " << s;

        if (s < 0) {
            emit error_message("TCP accept failed");
            std::this_thread::yield();
            continue;
        }

        // s is a new opened socket and should be closed after receive completed
        RAII_helper sock_auto_close([&]() {
            sock_accept = s;
        }, [&]() {
            close(sock_accept);
            sock_accept = -1;
        });

        int nsend;

        do {
            std::vector<uint8_t> query(MODBUS_TCP_MAX_ADU_LENGTH, 0);

            qDebug() << "Server receiving...";
            int nrcv = modbus_receive(ctx, &query[0]);
            qDebug() << "Server received " << nrcv;
            if (nrcv < 0) {
                break;
            }
            else if (nrcv < 10) {
                emit error_message(QString("Modbus server received < 10 bytes (%1)").arg(nrcv));
                nsend = modbus_reply_exception(ctx, &query[0], MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
            }
            else {
                query.resize(nrcv);
                nsend = server_reply(query);

                if (nsend < 0) {
                    emit error_message(QString("Modbus sending error - %1").arg(modbus_strerror(errno)));
                } else {
                    qDebug() << "Replying to request bytes : " << nsend;
                }
            }

        } while(nsend > 0);
    }

    qDebug() << "Server task finished";
}

int server_session::server_reply(const std::vector<uint8_t>& query)
{
    unsigned unitid = query[6];
    unsigned cmd = query[7];

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

    return modbus_reply(ctx, &query[0], query.size(), mb_map);
}

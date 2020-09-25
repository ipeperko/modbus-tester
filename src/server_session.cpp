#include "server_session.h"
#include <sys/socket.h>
#include <sstream>
#include <QDebug>

server_session::server_session(int port)
{
    mb_map = modbus_mapping_new(buffer_size_coils, buffer_size_coils, buffer_size_holding_register, buffer_size_input_registers);

    ctx = modbus_new_tcp("0.0.0.0", port);
    if (!ctx) {
        on_error("Cannot create new Modbus tcp connection");
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
            on_task_error(e.what());
        }
    });
}

void server_session::stop_server()
{
    // Disable task execution
    do_run = false;

    // Shutdown socket
    if (socket >= 0) {
        shutdown(socket, SHUT_RDWR);
        //close(socket);
        socket = -1;
    }

    // Join thread
    if (thr.joinable()) {
        thr.join();
    }
}


void server_session::task()
{
    qDebug() << "Server task started";

    socket = modbus_tcp_listen(ctx, max_connections);
    if (socket < 0) {
        on_error("Modbus tcp server socket failed");
    }

    while (do_run) {
        qDebug() << "Server accepting...";
        int rc = modbus_tcp_accept(ctx, &socket);
        qDebug() << "Server accept code : " << rc;
        if (rc < 0) {
            qWarning() << "TCP accept failed";
            std::this_thread::yield();
            continue;
        }

        int sent;

        do {
            sent = 0;
            std::vector<uint8_t> query(MODBUS_TCP_MAX_ADU_LENGTH, 0);

            qDebug() << "Server receiving...";
            int nrcv = modbus_receive(ctx, &query[0]);
            qDebug() << "Server received " << nrcv;
            if (nrcv < 10) {
                qWarning() << "Modbus server received < 10 bytes (" << nrcv << ") - break";
                break;
            }

            query.resize(nrcv);
            sent = modbusServerReply(query);

            if (sent < 0) {
                qWarning() << "Modbus sending error - %s" << modbus_strerror(errno);
            } else {
                qDebug() << "Replying to request bytes : " << sent;
            }

        } while(sent > 0);
    }

    qDebug() << "Server task finished";
}

int server_session::modbusServerReply(const std::vector<uint8_t>& query)
{
    int sent = 0;
    unsigned unitid = query[6];
    unsigned cmd = query[7];

    {
        std::ostringstream os;
        os << "Received cmd " << cmd << " (0x" << std::hex << cmd << std::dec <<
        ") from device id 0x" << std::hex << unitid << std::dec <<
        " (num bytes : " << query.size() << ") : ";

        for (auto it : query) {
            os << std::hex << static_cast<int>(it) << std::dec << " ";
        }

        //qDebug() << os.str().c_str();
        emit message_emitter.message(os.str().c_str());
    }

    sent = modbus_reply(ctx, &query[0], query.size(), mb_map);
    return sent;
}

void server_session::on_task_error(std::string const& what)
{
    //qWarning() << msg;
    emit message_emitter.message(what.c_str());
}

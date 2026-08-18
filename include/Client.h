#pragma once
#include <vector> // for worker threads on main
#include <thread> // threading
#include <list> // for worker threads on main
#include "../include/Connection.h"

class Client {
    public:
        // Client
        // Unix Domain
        void client_send_data_server_connection_unix_domain(const char* data, Connection &connection);

        // TCP/IP Domain
        void client_send_data_server_connection_tcp_domain(const char* data, Connection &connection);
};
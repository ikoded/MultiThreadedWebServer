#pragma once
#include <unistd.h> // for unlink
#include <sys/socket.h>
#include <sys/un.h> // Local AF_UNIX
#include <iostream>
#include <chrono>
#include <thread>
#include <filesystem>

class Connection{
    public:
        // Getter and Setters
        void setAddrUn(struct sockaddr_un addr);
        struct sockaddr_un getAddrUn();

        void setClientFd(int client_fd);
        int getClientFd();

        void setServerFd(int server_fd);
        int getServerFd();

        // UNIX Domains request functions
        // server
        void server_connection_unix_domain(const char* SOCKET_PATH);
        void server_read_data_client_connection_unix_domain();
        // client
        void client_send_data_server_connection_unix_domain(const char* data);
    private:
        struct sockaddr_un addr_un;
        int client_fd;
        int server_fd;
};
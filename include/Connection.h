#pragma once
#include <unistd.h> // for unlink
#include <sys/socket.h> // for Connection AND Client, creating socker,binding,listening,accepting
#include <sys/un.h> // Local AF_UNIX
#include <iostream> // standard out
#include <filesystem> // filesystem for checking UNIX Domain socket (for fun)
#include <fstream> // for Connection AND Client, ensures thread safe locks
#include <atomic> // thread safe counter

class Connection{
    public:
        // Getter and Setters
        struct sockaddr_un getAddrUn();
        void setAddrUn(struct sockaddr_un addr);

        int getClientFd();
        void setClientFd(int client_fd);

        int getServerFd();
        void setServerFd(int server_fd);

        int getClientsProccessed();
        void setClientsProccessed(int clients);
        void incrementClientsProccessed();

        bool getServerReady();
        void setServerReady(bool server_ready);

        // UNIX Domains request functions
        // server
        void server_connection_unix_domain(const char* SOCKET_PATH, const int MAX_CLIENT_THREADS);
        bool server_read_data_client_connection_unix_domain();
        
    private:
        struct sockaddr_un addr_un;
        int client_fd;
        int server_fd;
        std::atomic<int> clients_proccessed = 0; // smart variable that knows to handle concurrent data races
        bool server_ready = false;
};
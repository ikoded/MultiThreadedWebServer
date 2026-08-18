#pragma once
#include <unistd.h> // for unlink
#include <sys/socket.h> // for Connection AND Client, creating socker,binding,listening,accepting
#include <sys/un.h> // Local AF_UNIX
#include <iostream> // standard out
#include <filesystem> // filesystem for checking UNIX Domain socket (for fun)
#include <fstream> // for Connection AND Client, ensures thread safe locks
#include <atomic> // thread safe counter
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring> // for reinterpret cast

class Connection{
    public:
        // Getters (needed for external classes)
        struct sockaddr_un getAddrUn();

        struct sockaddr_in getAddrIn();

        int getClientsProccessed();

        bool getServerReady();

        // UNIX Domains request functions
        void server_connection_unix_domain(const int MAX_CLIENT_THREADS);

        // TCP/IP Domains request functions
        void server_connection_tcp_domain(const int MAX_CLIENT_THREADS);

        // Both Domains
        bool server_read_data_client_connection(std::string filename);
        Connection();
    private:
        // UNIX Domains
        struct sockaddr_un addr_un; // address of socket unix domain
        int client_fd; // setting clientfd for passing
        int server_fd; // setting serverfd for passing
        // TCP/IP Domains
        struct sockaddr_in IPv4Address{};
        // Global
        std::atomic<int> clients_proccessed = 0; // smart variable that knows to handle concurrent data races
        bool server_ready = false; // ensuring server is ready, not atomic since only one thread touches this
        // Configurables/Constants
        const char* SOCKET_PATH = "/tmp/mysocket"; // constant place for socket path
        char buffer[255] = {0}; // max buffer size of data
        
};
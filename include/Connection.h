#pragma once
#include <unistd.h> // for unlink
#include <sys/socket.h> // for Connection AND Client, creating socker,binding,listening,accepting
#include <sys/un.h> // Local AF_UNIX
#include <iostream> // standard out
#include <filesystem> // filesystem for checking UNIX Domain socket (for fun)
#include <fstream> // for Connection AND Client, ensures thread safe locks
#include <atomic> // thread safe counter
#include <netinet/in.h> // for sockaddr_in
#include <arpa/inet.h> // for inet_pton (presenation to network) to convert ip to binary
#include <cstring> // for reinterpret cast

class Connection{
    public:
        // Getters (needed for Client class)
        struct sockaddr_un getAddrUn();
        struct sockaddr_in getAddrIn();
        int getClientsAccepted();
        bool getServerReady();
        void setRawData(bool rawdata);
        bool getRawData();

        // UNIX Domain Server Start
        void server_connection_unix_domain(const int MAX_CLIENT_THREADS);
        // TCP Domain Server Start
        void server_connection_tcp_domain(const int MAX_CLIENT_THREADS);

        // UNIX/TCP Shared Functions
        void server_read_data_client_connection(std::string filename, int client_fd);
        Connection(int MAX_CLIENT_THREADS);
    private:
        // UNIX Domain
        // {} means to initalize which with sockaddr_un/in this will zero out struct
        // do this instead of memset to zero it out
        struct sockaddr_un addr_un{}; // address of socket unix domain
        const char* SOCKET_PATH = "/tmp/mysocket"; // constant place for socket path
        // TCP Domain
        struct sockaddr_in IPv4Address{};
        // Shared Variables
        int server_fd; // setting serverfd for passing
        int MAX_WORKER_THREADS; // will always equal MAX_CLIENT_THREADS
        std::atomic<int> clients_accepted{0}; // smart variable that knows to handle concurrent data races
        std::atomic<bool> server_ready = false; // ensuring server is ready, atomic in case race conditions
        bool rawdata = true; // used for tcp domain, this means raw data is sent by default       
};
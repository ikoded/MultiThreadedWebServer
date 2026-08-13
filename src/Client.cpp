#include "../include/Client.h"

/*

UNIX DOMAIN FUNCTIONS

*/

// Client

void Client::client_send_data_server_connection_unix_domain(const char* data, Connection &connection){
    struct sockaddr_un addr = connection.getAddrUn();
    // open socket for client
    int local_client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(connect(local_client_fd, (struct sockaddr*)&addr, sizeof(addr)) != 0){ // unexpected error
        std::cout << "Client Thread (" << std::this_thread::get_id() << "): Could not connect." << std::endl;
        close(local_client_fd);
        return;
    }
    send(local_client_fd, data, strlen(data),0);
    // close local socket as this is different from one on line 75
    close(local_client_fd);
}

/*

TCP/IP DOMAIN FUNCTIONS
(TODO)

*/
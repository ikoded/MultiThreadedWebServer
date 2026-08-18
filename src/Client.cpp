#include "../include/Client.h"

/*

UNIX DOMAIN FUNCTIONS

*/

void Client::client_send_data_server_connection_unix_domain(const char* data, Connection &connection){
    struct sockaddr_un addr = connection.getAddrUn();
    int local_client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    bool success = true;

    if(local_client_fd == -1){
        std::cout << "Client Thread (" << std::this_thread::get_id() << "): Could not create socket." << std::endl;
        success = false;
    }
    else if(connect(local_client_fd, (struct sockaddr*)&addr, sizeof(addr)) != 0){
        std::cout << "Client Thread (" << std::this_thread::get_id() << "): Could not connect." << std::endl;
        success = false;
    }
    else if(send(local_client_fd, data, strlen(data), 0) == -1){
        std::cout << "Client Thread (" << std::this_thread::get_id() << "): Could not send data." << std::endl;
        success = false;
    }

    if(local_client_fd != -1) close(local_client_fd);  // only close valid fd
}

/*

TCP/IP DOMAIN FUNCTIONS

*/

void Client::client_send_data_server_connection_tcp_domain(const char* data, Connection &connection){
    struct sockaddr_in addr = connection.getAddrIn();
    int local_client_fd = socket(AF_INET, SOCK_STREAM, 0);
    bool success = true;

    if(local_client_fd==-1){
        std::cout << "Client Thread (" << std::this_thread::get_id() << "): Could not create socket." << std::endl;
        success = false;
    }else if(connect(local_client_fd, (struct sockaddr*)&addr, sizeof(&addr)) == -1){
        std::cout << "Client Thread (" << std::this_thread::get_id() << "): Could not create connect." << std::endl;
        success = false;
    }else if(send(local_client_fd, data, strlen(data),0) == -1){
        std::cout << "Client Thread (" << std::this_thread::get_id() << "): Could not send data." << std::endl;
        success = false;
    }

    if(local_client_fd!=-1) close(local_client_fd); // only close if valid fd
}
#include "../include/Client.h"

/*

UNIX DOMAIN FUNCTION

*/

void Client::client_send_data_server_connection_unix_domain(const char* data, Connection &connection){
    // grab sockaddr_un
    struct sockaddr_un addr = connection.getAddrUn();
    // open socket using same AF_UNIX/SOCK_STREAM as Server
    int local_client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    bool success = true;

    if(local_client_fd == -1){
        std::cerr << "Client Thread (" << std::this_thread::get_id() << "): Could not create socket." << std::endl;
        success = false;
    }
    // converts to C friendly sockaddr, does not need reinterpret cast
    else if(connect(local_client_fd, (struct sockaddr*)&addr, sizeof(addr)) != 0){
        std::cerr << "Client Thread (" << std::this_thread::get_id() << "): Could not connect." << std::endl;
        success = false;
    }
    else if(send(local_client_fd, data, strlen(data), 0) == -1){
        std::cerr << "Client Thread (" << std::this_thread::get_id() << "): Could not send data." << std::endl;
        success = false;
    }

    if(local_client_fd != -1) close(local_client_fd);  // only close valid fd
}

/*

TCP DOMAIN FUNCTION

*/

void Client::client_send_data_server_connection_tcp_domain(const char* data, Connection &connection){
    // grab IPv4Address that has struct for sockaddr_in
    struct sockaddr_in addr = connection.getAddrIn();
    // create a client to connect to server thread using AF_INET
    int local_client_fd = socket(AF_INET, SOCK_STREAM, 0);
    bool success = true;

    if(local_client_fd==-1){
        std::cerr << "Client Thread (" << std::this_thread::get_id() << "): Could not create socket." << std::endl;
        success = false;
    // reinterpret cast is needed to treat the sockaddr_in as sockaddr
    }else if(connect(local_client_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1){
        std::cerr << "Client Thread (" << std::this_thread::get_id() << "): Could not connect." << std::endl;
        success = false;
    }else if(send(local_client_fd, data, strlen(data),0) == -1){
        std::cerr << "Client Thread (" << std::this_thread::get_id() << "): Could not send data." << std::endl;
        success = false;
    }

    if(local_client_fd!=-1) close(local_client_fd); // only close if valid fd
}
#include "../include/Connection.h"

/*

GETTERS & SETTERS

*/

void Connection::setAddrUn(struct sockaddr_un addr){
    this->addr_un = addr;
}

struct sockaddr_un Connection::getAddrUn(){
    return this->addr_un;
}

void Connection::setClientFd(int client_fd){
    this->client_fd = client_fd;
}

int Connection::getClientFd(){
    return this->client_fd;
}

void Connection::setServerFd(int server_fd){
    this->server_fd = server_fd;
}

int Connection::getServerFd(){
    return this->server_fd;
}

/*

UNIX DOMAIN FUNCTIONS

*/

// Server

void Connection::server_connection_unix_domain(const char* SOCKET_PATH){
    struct sockaddr_un addr;

    std::cout << "Server Thread: Creating socket" << std::endl;
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0); // (DOMAIN TYPE PROTOCOL)
    // if this fails server_fd is -1
    if(server_fd==-1){
        std::cout << "Server Thread: Failed to create socket with:\n" << AF_UNIX << "\n" << SOCK_STREAM << std::endl;
        exit(1);
    }

    memset(&addr, 0 , sizeof(addr)); // Set every byte in addr to 0, uses num bytes of struct sockaddr_un
    // AF_UNIX is a unix domain socket
    // this means it is not tcp/ip server so it cannot be used for anything than your own system
    // TODO: will create new connection test with tcp/ip after I test with unix domain
    // this means using AF_INET/6 and sockaddr_in
    addr.sun_family = AF_UNIX;
    // strncpy will not write past array bounds
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path)- 1); // -1 is room for terminating null byte
    // make sure file does not exist already so no errors happen about used address
    unlink(SOCKET_PATH);

    std::cout << "Server Thread: Binding to " << server_fd << " at " << &addr << std::endl;
    // bind the socket to the address with size of addr struct
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));

    listen(server_fd,1); // listens for client requests, only 1 request but this will be changed

    // now connection is open set server_fd and sockaddr_un
    setServerFd(server_fd);
    setAddrUn(addr);

    // accept a client request and return client file descriptor if not -1 (worked)
    std::cout << "Server Thread: Allowing client(s) to access" << std::endl;
    int server_client_fd = accept(server_fd,NULL,NULL); // will wait here until it comes in
    if(server_client_fd==-1){ // unexpected error
        std::cout << "Server Thread: Error accepting client" << std::endl;
        exit(1);
    }
    // set client fd for reference, will need changed when testing many threads/requests
    setClientFd(server_client_fd);
    // this means it connected
    std::cout << "Server Thread: Client connected at " << server_client_fd << std::endl;

    // read the data from the client
    server_read_data_client_connection_unix_domain();

    // close these as they are no longer needed
    close(server_fd);
    close(server_client_fd);
}

void Connection::server_read_data_client_connection_unix_domain(){
    int client_fd = getClientFd();

    // client data max buffer size, max 255 char
    char buffer[255] = {0};
    // try to recieve data now
    if(recv(client_fd, buffer, (sizeof(buffer) - 1), 0) != -1){
        std::cout << "Server Thread: Client data received, `" << buffer << "`" << std::endl;
    }else{ // unexpected error
        std::cout << "Server Thread: Error recieving data from client " << client_fd << std::endl;
        exit(1);
    }
}

// Client

void Connection::client_send_data_server_connection_unix_domain(const char* data){
    struct sockaddr_un addr = getAddrUn();
    // open socket for client
    int local_client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    std::cout << "Client Thread: Attempting to connect as client " << local_client_fd << " at " << &addr << std::endl;
    if(connect(local_client_fd, (struct sockaddr*)&addr, sizeof(addr)) != 0){ // unexpected error
        std::cout << "Client Thread: Could not connect." << std::endl;
    }

    std::cout << "Client Thread: Sending data as client " << local_client_fd << std::endl;
    send(local_client_fd, data, strlen(data),0);
    // close local socket as this is different from one on line 75
    close(local_client_fd);
}

/*

TCP/IP DOMAIN FUNCTIONS
(TODO)

*/
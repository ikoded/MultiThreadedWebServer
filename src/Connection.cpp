#include "../include/Connection.h"
#include "../include/Client.h"

/*

GETTERS & SETTERS

*/

struct sockaddr_un Connection::getAddrUn(){
    return this->addr_un;
}

void Connection::setAddrUn(struct sockaddr_un addr){
    this->addr_un = addr;
}

int Connection::getClientFd(){
    return this->client_fd;
}

void Connection::setClientFd(int client_fd){
    this->client_fd = client_fd;
}

int Connection::getServerFd(){
    return this->server_fd;
}

void Connection::setServerFd(int server_fd){
    this->server_fd = server_fd;
}

int Connection::getClientsProccessed(){
    return this->clients_proccessed;
}

void Connection::setClientsProccessed(int clients_proccessed){
    this->clients_proccessed = clients_proccessed;
}

void Connection::incrementClientsProccessed(){
    this->clients_proccessed++;
}

bool Connection::getServerReady(){
    return this->server_ready;
}

void Connection::setServerReady(bool server_ready){
    this->server_ready = server_ready;
}

/*

UNIX DOMAIN FUNCTIONS

*/

// Server

void Connection::server_connection_unix_domain(const char* SOCKET_PATH, const int MAX_CLIENT_THREADS){
    struct sockaddr_un addr;
    char buffer[255] = {0};
    std::ofstream myFile("../data/thread-message.txt", std::ios::app);
    myFile << "RUN WITH " << MAX_CLIENT_THREADS << " MAX CLIENT THREADS" << "\n";
    myFile.close();

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
    if(bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1){
        close(server_fd);
        std::cout << "Server binding error";
        return;
    }

    // listens for client requests
    if(listen(server_fd,MAX_CLIENT_THREADS)==-1){
        close(server_fd);
        std::cout << "Server listening error";
        return;
    }

    // now connection is open set server_fd and sockaddr_un
    setServerFd(server_fd);
    setAddrUn(addr);
    setServerReady(true);

    // will eventually leave if max requests hit
    while(true && getClientsProccessed() < MAX_CLIENT_THREADS){
        // accept a client request and return client file descriptor if not -1 (worked)
        std::cout << "Server Thread: Allowing client(s) to access" << std::endl;
        int server_client_fd = accept(server_fd,nullptr,nullptr); // will wait here until it comes in
        if(server_client_fd==-1){ // unexpected error
            std::cout << "Server Thread: Error accepting client" << std::endl;
            close(server_client_fd);
            break;
        }
        // set client fd for reference, will need changed when testing many threads/requests
        setClientFd(server_client_fd);
        // this means it connected
        std::cout << "Server Thread: Client connected at " << server_client_fd << std::endl;
        // read the data from the clients
        if(server_read_data_client_connection_unix_domain() == false){
            // could not read properly
            close(server_client_fd);
            break;
        }

        close(server_client_fd);
    }

    myFile.open("../data/thread-message.txt", std::ios::app);
    myFile << "\n\n";
    myFile.close();

    // close these as they are no longer needed
    close(server_fd);
}

bool Connection::server_read_data_client_connection_unix_domain(){
    int client_fd = getClientFd();

    // client data max buffer size, max 255 char
    char buffer[255] = {0};
    // try to recieve data now
    if(recv(client_fd, buffer, (sizeof(buffer) - 1), 0) != -1){
        std::ofstream myFile("../data/thread-message.txt", std::ios::app);
        myFile << buffer << " "; // this can come in random so each run looks different
        myFile.close();

        incrementClientsProccessed(); // increment so we can tell how many threads server actually worked on, end metrics
        return true;
    }else{ // unexpected error
        std::cout << "Server Thread: Error receiving data from client " << client_fd << std::endl;
        
        return false;
    }
}

/*

TCP/IP DOMAIN FUNCTIONS
(TODO)

*/
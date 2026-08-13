#include "../include/Connection.h"
#include "../include/Client.h"

/*

GETTERS

*/

struct sockaddr_un Connection::getAddrUn(){
    return this->addr_un;
}

int Connection::getClientsProccessed(){
    return this->clients_proccessed;
}


bool Connection::getServerReady(){
    return this->server_ready;
}

/*

UNIX DOMAIN FUNCTIONS

*/

// Server

void Connection::server_connection_unix_domain(const int MAX_CLIENT_THREADS){
    // file for fun and tracking order it comes back
    std::ofstream myFile("data/thread-message.txt", std::ios::app);
    myFile << "RUN WITH " << MAX_CLIENT_THREADS << " MAX CLIENT THREADS" << "\n";
    myFile.close(); // close file as it will be used in loop

    std::cout << "Server Thread: Creating socket" << std::endl;
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0); // (DOMAIN TYPE PROTOCOL)
    // if this fails server_fd is -1
    if(server_fd==-1){
        std::cout << "Server Thread: Failed to create socket with:\n" << AF_UNIX << "\n" << SOCK_STREAM << std::endl;
        exit(1);
    }

    std::cout << "Server Thread: Binding to " << server_fd << " at " << &addr_un << std::endl;
    // bind the socket to the address with size of addr struct
    if(bind(server_fd, (struct sockaddr*)&addr_un, sizeof(addr_un)) == -1){
        close(server_fd);
        std::cout << "Server Thread: Server binding error";
        return;
    }

    // listens for client requests
    if(listen(server_fd,MAX_CLIENT_THREADS)==-1){
        close(server_fd);
        std::cout << "Server Thread: Server listening error";
        return;
    }

    // now connection is open set server_fd, sockaddr_un and server_ready
    this->server_fd = server_fd;
    this->server_ready = true;

    // will eventually leave if max clients hit (tweak on main, max 10 unless you add more messages)
    while(true && (this->clients_proccessed) < MAX_CLIENT_THREADS){
        // accept a client request and return client file descriptor if not -1 (worked)
        std::cout << "Server Thread: Allowing client(s) to access" << std::endl;
        int server_client_fd = accept(server_fd,nullptr,nullptr); // will wait here until it comes in
        if(server_client_fd==-1){ // unexpected error
            std::cout << "Server Thread: Error accepting client" << std::endl;
            close(server_client_fd);
            break;
        }
        // set client fd for reference
        client_fd = server_client_fd;
        // this means it connected
        std::cout << "Server Thread: Client connected at " << server_client_fd << std::endl;
        // read the data from the client
        if(server_read_data_client_connection_unix_domain() == false){
            // could not read properly
            close(server_client_fd);
            break;
        }
        // close client when done
        close(server_client_fd);
    }

    // add new lines to end for clarity in next runs
    myFile.open("data/thread-message.txt", std::ios::app);
    myFile << "\n\n";
    myFile.close();

    close(server_fd);

    // end server function with clean up
    unlink(SOCKET_PATH);
}

bool Connection::server_read_data_client_connection_unix_domain(){
    // get client fd from current client fd in loop line 113
    int client_fd = this->client_fd;

    // client data max buffer size, max 255 char
    // probably should be global but I don't intend pushing this to limits for now
    char buffer[255] = {0};
    // try to recieve data now
    if(recv(client_fd, buffer, (sizeof(buffer) - 1), 0) != -1){
        std::ofstream myFile("data/thread-message.txt", std::ios::app); // append to file
        myFile << buffer << " "; // this can come in random so each run looks different after first
        myFile.close(); // make sure to close

        this->clients_proccessed++; // increment so we can tell how many threads server actually worked on, end metrics
        return true;
    }else{ // unexpected error
        std::cout << "Server Thread: Error receiving data from client " << client_fd << std::endl;
        return false;
    }
}

Connection::Connection(){
    // make sure file does not exist already so no errors happen about used address
    unlink(SOCKET_PATH);
    // sockaddr_un is the address structure used for AF_UNIX sockets
    // It stores a local filesystem path, not an IP address or port
    // Zero the entire struct to avoid garbage values in padding/unused fields
    memset(&addr_un, 0, sizeof(addr_un));

    // AF_UNIX means this is a local socket, not TCP/IP
    addr_un.sun_family = AF_UNIX;
    // sun_path is the filesystem path used by the socket
    strncpy(addr_un.sun_path, SOCKET_PATH, sizeof(addr_un.sun_path) - 1);
    // add null terminator
    addr_un.sun_path[sizeof(addr_un.sun_path) - 1] = '\0';
}

/*

TCP/IP DOMAIN FUNCTIONS
(TODO)

*/
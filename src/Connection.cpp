#include "../include/Connection.h"
#include "../include/Client.h"

/*

GETTERS

*/

struct sockaddr_un Connection::getAddrUn(){
    return addr_un;
}

struct sockaddr_in Connection::getAddrIn(){
    return IPv4Address;
}

int Connection::getClientsAccepted(){
    return clients_accepted;
}


bool Connection::getServerReady(){
    return server_ready;
}

void Connection::setRawData(bool rawdata){
    this->rawdata = rawdata;
}

bool Connection::getRawData(){
    return rawdata;
}

/*

HTTP setup

*/

// prepare html data in http format
std::string prepareHTTP_HTML(){
    std::string html;
    std::string line;
    int contentlength = std::filesystem::file_size("data/test.html");
    std::ifstream myFile("data/test.html");
    
    while(std::getline(myFile,line)){
        html.append(line);
        html.append("\n");
    }
    
    std::string http = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: " + std::to_string(html.length()) + "\r\n"
    "Connection: close\r\n"
    "\r\n"
    + html;

    return http;
}

/*

UNIX DOMAIN SERVER START

*/

void Connection::server_connection_unix_domain(const int MAX_CLIENT_THREADS){
    // file for tracking each run to ensure data was properly sent
    // used multiple times and in reading client data method
    std::string filename = "data/unix-thread-message.txt";
    std::ofstream myFile(filename, std::ios::app);
    myFile << "RUN WITH " << MAX_CLIENT_THREADS << " MAX CLIENT THREADS" << "\n";
    myFile.close(); // close file as it will be used in loop

    std::cout << "Server Thread: Creating socket." << std::endl;
    // AF_UNIX for UNIX Domain
    // SOCK_STREAM works for both UNIX and TCP
    // SOCK_DGRAM would be for UDP, a datagram that preserves message boundaries unlike TCP
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0); // (DOMAIN TYPE PROTOCOL)
    // if this fails server_fd is -1
    if(server_fd==-1){
        std::cerr << "Server Thread: Failed to create socket with:\n" << AF_UNIX << "\n" << SOCK_STREAM << std::endl;
        return;
    }

    std::cout << "Server Thread: Binding to " << server_fd << " at " << &addr_un << "." << std::endl;
    // bind the socket to the address with size of addr struct
    // type cast to be C friendly sockaddr
    if(bind(server_fd, (struct sockaddr*)&addr_un, sizeof(addr_un)) == -1){
        close(server_fd);
        std::cerr << "Server Thread: Server binding error.";
        return;
    }

    std::cout << "Server Thread: Opening for listening." << std::endl;
    // listens for client requests of MAX_CLIENT_THREADS
    if(listen(server_fd,MAX_CLIENT_THREADS)==-1){
        close(server_fd);
        std::cerr << "Server Thread: Server listening error.";
        return;
    }

    // now connection is open set server_fd and server_ready
    this->server_fd = server_fd;
    server_ready = true;

    std::vector<std::thread> workerThreads;
    workerThreads.reserve(MAX_WORKER_THREADS);

    std::cout << "Server Thread: Allowing client(s) to access." << std::endl;
    // will eventually break loop if max clients hit OR an error happens
    while(clients_accepted < MAX_CLIENT_THREADS){
        // accept a client request and return client file descriptor if not -1
        // if you wanted the client sockadrr and address length pass variables before using reference
        int server_client_fd = accept(server_fd,nullptr,nullptr); // will wait here until it comes in
        if(server_client_fd==-1){ // unexpected error
            std::cerr << "Server Thread: Error accepting client." << std::endl;
            break;
        }
        // this means it connected
        std::cout << "Server Thread: Client connected at " << server_client_fd << "." << std::endl;
        // read the data from the client
        workerThreads.emplace_back(&Connection::server_read_data_client_connection, this, filename, server_client_fd);
        clients_accepted++; // increment so we can tell how many threads server actually worked on, end metrics
    }

    for(auto& th : workerThreads){
        if(th.joinable()) th.join();
    }

    // add new lines to end for clarity in next runs
    myFile.open(filename, std::ios::app);
    myFile << "\n\n";
    myFile.close();

    // if it makes it here, connection needs closed
    close(server_fd);

    // end server function with clean up of UNIX Domain directory
    unlink(SOCKET_PATH);
}

/*

TCP/IP DOMAIN SERVER START

*/

void Connection::server_connection_tcp_domain(const int MAX_CLIENT_THREADS){
    std::string filename;
    std::ofstream myFile(filename, std::ios::app);
    if(rawdata){ // only needed for raw data side
        // file to be used for data received
        filename = "data/tcp-thread-message.txt"; 
        myFile.open(filename, std::ios::app);
        myFile << "RUN WITH " << MAX_CLIENT_THREADS << " MAX CLIENT THREADS" << "\n";
        myFile.close(); // close file as it will be used in loop
    }

    std::cout << "Server Thread: Creating Socket." << std::endl;
    // SOCK_STREAM is default for TCP using AF_INET
    // AF_INET family address of IPv4 Addresses
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // server failed to start on socket
    if(server_fd == -1){
        std::cerr << "Server Thread: Failed to create socket with " << AF_INET << " & " << SOCK_STREAM << std::endl;
        return;
    }

    std::cout << "Server Thread: Binding to " << server_fd << " at " << &IPv4Address.sin_addr << "." << std::endl;
    // type casting to C friendly sockaddr, but reinterpret_case is necessary for sockaddr_in
    if(bind(server_fd, reinterpret_cast<sockaddr*>(&IPv4Address), sizeof(IPv4Address)) == -1){
        close(server_fd);
        std::cerr << "Server Thread: Failed to bind to " << server_fd << "." << std::endl;
        return;
    }

    std::cout << "Server Thread: Opening for listening." << std::endl;
    // listening for MAX_CLIENT_THREADS amount of requests
    if(listen(server_fd,MAX_CLIENT_THREADS)==-1){
        close(server_fd);
        std::cerr << "Server Thread: Server listening error." << std::endl;
        return;
    }

    // Set server fd and server open
    this->server_fd = server_fd;
    server_ready = true;

    std::cout << "Server Thread: Accepting client(s) to access." << std::endl;

    std::vector<std::thread> workerThreads;
    if(rawdata) workerThreads.reserve(MAX_WORKER_THREADS); // only needed for raw data

    // breaks once MAX_CLIENTS_THREADS amount of clients are listened to
    // or continue until control c for HTML server
    while(clients_accepted < MAX_CLIENT_THREADS || !rawdata){
        int server_client_fd = accept(server_fd, nullptr, nullptr); // waiting for client
        if(server_client_fd == -1){
            std::cerr << "Server Thread: Could not accept client." << std::endl;
            break;
        }
        std::cout << "Server Thread: Client connected at " << server_client_fd << "." << std::endl;

        if(!rawdata){ // send HTTP back with HTML
            // receive GET from client browser
            ssize_t received_bytes = recv(server_client_fd, buffer, sizeof(buffer)-1,0);
            std::string received_string(buffer, received_bytes);
            std::cout << "Browser sent: \n" << received_string << "\n";

            // get http with html to send back to browser client
            std::string http = prepareHTTP_HTML();
            // send the http header with html
            send(server_client_fd,http.c_str(),http.length(),0);
            // close client so it loads
            close(server_client_fd);
        }else{ // raw data
            workerThreads.emplace_back(&Connection::server_read_data_client_connection, this, filename, server_client_fd);
            clients_accepted++;
        }

        for(auto& th : workerThreads){
            if(th.joinable()) th.join();
        }
    }

    // add new lines to end for clarity in next runs
    myFile.open(filename, std::ios::app);
    myFile << "\n\n";
    myFile.close();

    close(server_fd);
}

/*

Both Unix & TCP/IP Domains

*/

void Connection::server_read_data_client_connection(std::string filename, int client_fd){
    std::string received_string;
    char buffer[8192] = {0}; // max buffer size of HTTP 8KB
    // try to recieve data now
    // ssize_t required for -1 handling, size_t is unsigned and therefore cannot be negative
    // sizeof(buffer) - 1 is saving room for null terminator
    ssize_t received_bytes;
    while((received_bytes = recv(client_fd, buffer, (sizeof(buffer)-1),0)) > 0){
        // this is needed to handle the null terminator automatically
        received_string.append(buffer,received_bytes);
    }

    if(received_bytes==-1){
        std::cerr << "Server Thread: Error receiving data from client " << client_fd << "." << std::endl;
    }else{
        std::ofstream myFile(filename, std::ios::app); // append to file
        // this can come in random so each run looks different after first
        // it is random due to threading, TCP ensures order within SAME connection
        myFile << received_string << " ";
        myFile.close(); // make sure to close
    }

    close(client_fd);
}

Connection::Connection(int MAX_CLIENT_THREADS){
    MAX_WORKER_THREADS = MAX_CLIENT_THREADS;
    // UNIX DOMAIN

    // make sure file does not exist already so no errors happen about used address
    unlink(SOCKET_PATH);
    // sockaddr_un is the address structure used for AF_UNIX sockets
    // it stores a local filesystem path, not an IP address or port

    // AF_UNIX means this is a local UNIX Domain socket
    addr_un.sun_family = AF_UNIX;
    // sun_path is the filesystem path used by the socket, copy SOCKET_PATH to sun_path
    strncpy(addr_un.sun_path, SOCKET_PATH, sizeof(addr_un.sun_path) - 1);
    // add null terminator for end of string
    addr_un.sun_path[sizeof(addr_un.sun_path) - 1] = '\0';

    // TCP/IP DOMAIN

    // sockaddr_in is the address structure used for TCP sockets
    // using wrapper instead of manipulating sa_data directly
    IPv4Address.sin_family = AF_INET;
    // host to network properly sets big endian 16 bits for port, 16 bit ports : [0-65535]
    IPv4Address.sin_port = htons(8080);
    // converts IP to proper binary for proper IPv4 addresses
    // AF_INET tells it IPv4 to handle, sin_addr gets this IP saved in binary
    inet_pton(AF_INET, "127.0.0.1", &IPv4Address.sin_addr);
}
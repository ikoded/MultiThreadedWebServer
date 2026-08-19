#include "../include/Connection.h"
#include "../include/Client.h"
#include <chrono>

// list of messages to send
std::list<const char*> messages = {"Hello_1",
    "Server_2",
    "This_3",
    "Is_4",
    "Your_5",
    "Captain_6",
    "Speaking_7",
    "Through_8",
    "Client_9",
    "Threads_10"
};
const int MAX_CLIENT_THREADS = 10; // 10 is max because only 10 messages I set up above

// method to print at end of program
void metrics(Connection &connection, auto execution_time){
    std::cout << "\nNumber of clients that succesfully contacted server: " << connection.getClientsProccessed() << " / " << MAX_CLIENT_THREADS << std::endl;
    std::cout << "Executed in " << execution_time << std::endl;
}

// choice menu
char chooseDomain(){
    char choice;
    std::cout << "1. Unix Domain\n2. TCP/IP Domain\nChoice(1-2): ";
    std::cin >> choice;
    std::cout << std::endl;

    return choice;
}

// start unix if 1 chosen
void startUnixDomain(Connection &connection, Client &client){
    // create connection at /tmp/mysocket, Server Thread
    std::thread serverThread(&Connection::server_connection_unix_domain,&connection, MAX_CLIENT_THREADS);

    // set up worker threads
    std::vector<std::thread> clientThreads;
    // wait for server to be ready
    while(!connection.getServerReady()){}
    clientThreads.reserve(MAX_CLIENT_THREADS); // reserve client memory
    for(int i = 0; i < MAX_CLIENT_THREADS; ++i){
        // get example message for each client
        const char* example_message = messages.front();
        messages.pop_front(); // pop this so you don't just grab front everytime
        // call worker threads
        clientThreads.emplace_back(&Client::client_send_data_server_connection_unix_domain, &client, example_message, std::ref(connection));
    }
    // make sure all clients finish first
    for(auto& th : clientThreads){
        if(th.joinable()) th.join();
    }
    // make sure server thread finishes
    if(serverThread.joinable()) serverThread.join();
}

// start tcp if 2 chosen
void startTCPDomain(Connection &connection, Client &client){
    // start server thread on localhost:8080
    std::thread serverThread(&Connection::server_connection_tcp_domain,&connection, MAX_CLIENT_THREADS);

    // setup cleint threads
    std::vector<std::thread> clientThreads;

    // wait for server to be ready
    while(!connection.getServerReady()){}
    clientThreads.reserve(MAX_CLIENT_THREADS); // reserve client memory
    for(int i = 0; i < MAX_CLIENT_THREADS; ++i){
        // get example for each client
        const char* example_message = messages.front();
        messages.pop_front(); // remove so we can continue on to next message in next iteration
        // allows program to create a client thread for each message it sends
        clientThreads.emplace_back(&Client::client_send_data_server_connection_tcp_domain, &client, example_message, std::ref(connection));
    }

    // clean up threads
    for(auto& th : clientThreads){
        if(th.joinable()) th.join();
    }
    if(serverThread.joinable()) serverThread.join();
}

int main(){
    std::chrono::steady_clock::time_point start_time; // placeholder
    Connection connection; // proccesses server connections
    Client client; // client that sends data to server

    // allow user to choose UNIX or TCP Domain
    switch (chooseDomain())
    {
    case '1':
        std::cout << "Starting Unix Domain..." << std::endl;
        start_time = std::chrono::steady_clock::now();
        startUnixDomain(connection, client);
        break;
    case '2':
        std::cout << "Starting TCP Domain..." << std::endl;
        start_time = std::chrono::steady_clock::now();
        startTCPDomain(connection, client);
        break;
    default:
        std::cout << "Wrong choice, exiting" << std::endl;
        break;
    }

    // print metrics and exit
    auto end_time = std::chrono::steady_clock::now();
    metrics(connection, (end_time-start_time));
    return 0;
}
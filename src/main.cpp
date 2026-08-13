#include "../include/Connection.h"
#include "../include/Client.h"
#include <chrono>

#define SOCKET_PATH "/tmp/mysocket"

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
const int MAX_CLIENT_THREADS = 10; // 10 is max because only 10 messages I set up

// Method to print at end of program
void metrics(Connection &connection, auto execution_time){
    std::cout << "\nNumber of clients that succesfully contacted server: " << connection.getClientsProccessed() << " / " << MAX_CLIENT_THREADS << std::endl;
    std::cout << "Executed in " << execution_time << std::endl;
}

int main(){
    // Start tracking execution time
    auto start_time = std::chrono::steady_clock::now();

    Connection connection; // Proccesses Server Connections
    Client client; // Client Threads that call to Server

    // Create connection at /tmp/mysocket, Server Thread
    std::thread serverThread(&Connection::server_connection_unix_domain,&connection, SOCKET_PATH, MAX_CLIENT_THREADS);

    // Set up worker threads and run
    std::vector<std::thread> clientThreads;
    // Wait for server to be ready
    while(!connection.getServerReady()){}
    clientThreads.reserve(MAX_CLIENT_THREADS); // Reserve client memory
    for(int i = 0; i < MAX_CLIENT_THREADS; ++i){
        // Get example message for each client
        const char* example_message = messages.front();
        messages.pop_front(); // Pop this so you don't just grab front everytime
        // Call worker threads
        clientThreads.emplace_back(&Client::client_send_data_server_connection_unix_domain, &client, example_message, std::ref(connection));
    }
    // Make sure all clients finish first
    for(auto& th : clientThreads){
        if(th.joinable()) th.join();
    }
    // Make sure server thread finishes
    if(serverThread.joinable()) serverThread.join();

    // Unlink it case spot is taken
    // Redundant because server does this at start but like to be safe
    unlink(SOCKET_PATH);

    auto end_time = std::chrono::steady_clock::now();
    metrics(connection, (end_time-start_time));
    return 0;
}
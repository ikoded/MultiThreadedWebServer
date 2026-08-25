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
    std::cout << "1. Unix Domain\n2. TCP Domain\nChoice(1-2): ";
    std::cin >> choice;
    std::cout << std::endl;

    return choice;
}

// start clients based on choice
// want this also to be flexible so I can utilize it for other functionality
void startClients(Connection &connection, Client &client, char choice){
    // set up worker threads
    std::vector<std::thread> clientThreads;
    // wait for server to be ready
    // TODO: Change this to something akin to mutex lock so cpu does not waste cycles
    while(!connection.getServerReady()){}
    clientThreads.reserve(MAX_CLIENT_THREADS); // reserve client memory
    for(int i = 0; i < MAX_CLIENT_THREADS; ++i){
        // get example message for each client
        const char* example_message = messages.front();
        messages.pop_front(); // pop this so you don't just grab front everytime
        // call worker threads
        if(choice=='1'){ // unix client
            clientThreads.emplace_back(&Client::client_send_data_server_connection_unix_domain, &client, example_message, std::ref(connection));
        }else{ // tcp client
            clientThreads.emplace_back(&Client::client_send_data_server_connection_tcp_domain, &client, example_message, std::ref(connection));
        }
    }
    // make sure all clients finish first
    for(auto& th : clientThreads){
        if(th.joinable()) th.join();
    }
}

// starts domain based on choice
// want this to be flexible so I can use it for other functionality
void startDomain(Connection &connection, Client &client, char choice){
    // create connection at /tmp/mysocket, Server Thread
    std::thread serverThread;
    if(choice=='1'){ // unix domain
        serverThread = std::thread(&Connection::server_connection_unix_domain,&connection, MAX_CLIENT_THREADS);
    }else{ // tcp domain
        serverThread = std::thread(&Connection::server_connection_tcp_domain,&connection,MAX_CLIENT_THREADS);
    }

    // clients are only needed for raw data
    if(connection.getRawData()) startClients(connection, client, choice);
    
    // make sure server thread finishes
    if(serverThread.joinable()) serverThread.join();
}

// used for tcp domain
bool chooseDataSent(){
    char choice;

    std::cout << "Do you want to serve HTML? (y/n): ";
    std::cin >> choice;
    while(choice!='y' && choice!='n'){
        std::cout << "\nPlease enter y/n: ";
        std::cin >> choice;
    }

    return choice == 'y' ? true : false;
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
        startDomain(connection, client, '1'); // 1 is hardcoded for unix domain

        break;
    case '2':
        std::cout << "Starting TCP Domain..." << std::endl;
        start_time = std::chrono::steady_clock::now();
        if(chooseDataSent()){
            connection.setRawData(false); // set this so it uses HTTP
        }
        startDomain(connection, client, '2'); // 2 is hardcoded for tcp domain   
        
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
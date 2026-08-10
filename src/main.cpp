#include "../include/Connection.h"

#define SOCKET_PATH "/tmp/mysocket"

int main(){
    Connection connection;
    // create connection at /tmp/mysocket, Server Thread
    std::thread serverThread(&Connection::server_connection_unix_domain,&connection,SOCKET_PATH);
    // sleep so it can be up and ready
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // just testing socket is open
    std::string opensocket = std::filesystem::is_socket(SOCKET_PATH) == 1 ? "true" : "false";
    std::cout << "Socket open: " << opensocket << std::endl;

    if(opensocket == "true"){
        // Set message and send data
        const char* example_message = "Helllooo server";
        connection.client_send_data_server_connection_unix_domain(example_message);
    }

    // make sure thread finishes
    if(serverThread.joinable()) serverThread.join();

    // unlink it case spot is taken
    unlink(SOCKET_PATH);
    return 0;
}
#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <winsock2.h>  // Windows sockets
#include <ws2tcpip.h>  // Additional functions for sockets
#include <iostream>

// Link with the Ws2_32.lib library
#pragma comment(lib, "Ws2_32.lib")

class Server {
public:
    explicit Server(const std::string& port);  // Constructor
    SOCKET start();                            // Start the server
    void stop(SOCKET server_socket_fd);        // Stop the server
    ~Server();                                 // Destructor

private:
    std::string port;  // Port to listen on
};

#endif  // SERVER_H
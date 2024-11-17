#include "server.h"
#include <cstdlib>  // For exit()

// Constructor
Server::Server(const std::string& port) {
    this->port = port;
}

// Start the server
SOCKET Server::start() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        exit(1);
    }

    // Create a socket
    SOCKET server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd == INVALID_SOCKET) {
        std::cerr << "Failed to create server socket. Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(1);
    }

    // Configure server socket address
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(std::stoi(port));
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces

    // Bind the socket
    if (bind(server_socket_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind server socket. Error: " << WSAGetLastError() << std::endl;
        closesocket(server_socket_fd);
        WSACleanup();
        exit(1);
    }

    // Start listening on the socket
    if (listen(server_socket_fd, 5) == SOCKET_ERROR) {
        std::cerr << "Failed to listen on server socket. Error: " << WSAGetLastError() << std::endl;
        closesocket(server_socket_fd);
        WSACleanup();
        exit(1);
    }

    std::cout << "Server started on port: " << port << std::endl;
    return server_socket_fd;
}

// Stop the server
void Server::stop(SOCKET server_socket_fd) {
    // Close the server socket
    closesocket(server_socket_fd);

    // Clean up Winsock
    WSACleanup();

    std::cout << "Server stopped." << std::endl;
}

// Destructor
Server::~Server() {
    // No explicit resource cleanup here; handled in stop()
}

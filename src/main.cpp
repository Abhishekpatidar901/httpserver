#include "server.h"
#include "request.cpp"
#include "response.cpp"
#include <thread>
#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

// Link with the Ws2_32.lib library
#pragma comment(lib, "Ws2_32.lib")

// Utility function to log the serving of a file.
void logServingFile(const std::string& path, const std::string& mimetype) {
    std::cout << "Serving file: " << path << " with MIME type: " << mimetype << std::endl;
}

// Function to handle client requests
void handleClient(SOCKET client_socket_fd) {
    char client_req_buffer[1024];

    // Receive the request from the client
    int bytes_received = recv(client_socket_fd, client_req_buffer, sizeof(client_req_buffer) - 1, 0);
    if (bytes_received <= 0) {
        std::cerr << "Failed to read from client." << std::endl;
        closesocket(client_socket_fd);
        return;
    }

    client_req_buffer[bytes_received] = '\0';  // Null-terminate the received data

    HttpRequest req;
    req.parseRequest(client_req_buffer);

    // Get MIME type and read the requested file
    std::string mimetype = req.getMimeType(req.path);
    std::string body = req.readHtmlFile(req.path);

    HttpResponse res;
    std::string response = res.frameHttpResponse("200", "OK", req.headers, body, mimetype);

    logServingFile(req.path, mimetype);

    // Send the response to the client
    int bytes_sent = send(client_socket_fd, response.c_str(), response.length(), 0);
    if (bytes_sent == SOCKET_ERROR) {
        std::cerr << "Failed to send response to client. Error: " << WSAGetLastError() << std::endl;
    }

    // Close the client socket
    closesocket(client_socket_fd);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    // Initialize the server
    Server server(argv[1]);
    sockaddr_in client_addr{};
    int s_fd = server.start();
    int client_addr_size = sizeof(client_addr);

    while (true) {
        // Accept a client connection
        SOCKET client_socket_fd = accept(s_fd, (sockaddr*)&client_addr, &client_addr_size);
        if (client_socket_fd == INVALID_SOCKET) {
            std::cerr << "Failed to accept client request. Error: " << WSAGetLastError() << std::endl;
            continue;
        }

        // Create a new thread to handle the client
        std::thread clientThread(handleClient, client_socket_fd);
        clientThread.detach();  // Detach the thread to allow it to run independently
    }

    // Stop the server (this code is never reached due to infinite loop)
    server.stop(s_fd);

    return 0;
}

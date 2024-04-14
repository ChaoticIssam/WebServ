#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "Webserv.h"


#define PORT 8080
#define MAX_EVENTS 10
#define MAX_BUFFER 1024

void setNonBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    // Create socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // Set socket options
    int reuse = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        std::cerr << "Error setting SO_REUSEADDR" << std::endl;
        close(serverSocket);
        return 1;
    }

    // Prepare sockaddr_in structure
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Error binding" << std::endl;
        close(serverSocket);
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, SOMAXCONN) == -1) {
        std::cerr << "Error listening" << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    // Create epoll instance
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        std::cerr << "Error creating epoll instance" << std::endl;
        close(serverSocket);
        return 1;
    }

    // Add the server socket to epoll
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = serverSocket;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverSocket, &event);

    while (true) {
        // Wait for events
        struct epoll_event events[MAX_EVENTS];
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < num_events; ++i) {
            if (events[i].data.fd == serverSocket) {
                // Accept new connection
                struct sockaddr_in clientAddress;
                socklen_t clientAddresslen = sizeof(clientAddress);
                int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddresslen);

                if (clientSocket == -1) {
                    std::cerr << "Error accepting connection" << std::endl;
                    continue;
                }

                setNonBlocking(clientSocket);

                // Add the new client socket to epoll
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = clientSocket;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientSocket, &event);
            } else {
                // Handle existing client
                int clientSocket = events[i].data.fd;
                char buffer[MAX_BUFFER];
                int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

                ///////////////////////////
                // // Request handle // //
                std::string newBuffer(buffer);
                if (newBuffer.empty()) {
                    std::cout << "Invalid request " << std::endl;
                    const char* response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\n"
                                    "<html><body><h1>400 Bad Request!\n</h1></body></html>";
                    send(clientSocket, response, strlen(response), 0);
                    close(clientSocket);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clientSocket, &event);
                }
                RequestParser parser;
                Request http = parser.requestParse(newBuffer);
                std::cout << "===================\n";
                parser.requestPrint(http);
                std::cout << "===================\n";
                try
                {
                    requestHandle(http);
                }
                catch(const std::exception& e)
                {
                    std::cerr << "Exception caught: " << e.what() << std::endl;
                }
                /////////////////////////

                if (bytesRead <= 0) {
                    std::cerr << "Error reading from client" << std::endl;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, clientSocket, &event);
                    close(clientSocket);
                } else {
                    // Null-terminate the received data
                    buffer[bytesRead] = '\0';
                    // std::cout << "Received from client " << clientSocket << ": " << buffer << std::endl;

                    // Respond to the client
                    const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, client!";
                    send(clientSocket, response, strlen(response), 0);
                }
            }
        }
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}

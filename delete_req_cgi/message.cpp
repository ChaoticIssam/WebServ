#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>
#include <map>
#include <random>

#define MAX_EVENTS 10
#define MAX_CLIENTS 10

#define PORT 8080
#define PORT1 8080
#define PORT2 8080

int main()
{
    int serverFd[3], epollFd;
    struct sockaddr_in serverAddress;
    struct epoll_event event, events[MAX_EVENTS];
    std::vector<int> ports = {8080, 8081, 8082};

    int i = 0;
    while (i < 3)
    {
        serverFd[i] = socket(AF_INET, SOCK_STREAM, 0);
        if (serverFd[i] == -1)
        {
            std::cerr << "Failed to create socket." << std::endl;
            return 1;
        }
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(ports[i]);

        if (bind(serverFd[i], (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
        {
            std::cerr << "Failed to bind socket on port " << ports[i] << std::endl;
            close(serverFd[i]);
            return 1;
        }

        if (listen(serverFd[i], MAX_CLIENTS) == -1) {
            std::cerr << "Failed to listen." << std::endl;
            close(serverFd[i]);
            return 1;
        }
        i++;
    }
    epollFd = epoll_create(1);
    if (epollFd == -1)
    {
        std::cerr << "Failed to create epoll instance." << std::endl;
        for (int j = 0; j < i; j++)
        {
            close(serverFd[j]);
        }
        return 1;
    }
    i = 0;
    while (i < 3)
    {
        event.events = EPOLLIN;
        event.data.fd = serverFd[i];
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd[i], &event) == -1)
        {
            std::cerr << "Failed to add server socket " << serverFd[i] << " to epoll instance." << std::endl;
            for (int j = 0; j < 3; j++)
            {
                close(serverFd[j]);
            }
            close(epollFd);
            return 1;
        }
        i++;
    }
    for (int j = 0; j < ports.size() ;j++)
    {
        std::cout << "Server started. Listening on port " << ports[j] << std::endl;
    }
    int connectedClients = 0;
    std::vector<int> clientSockets;
    std::map<int,int> client_map;
    while (1)
    {
       int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, -1);
        if (numEvents == -1)
        {
            std::cerr << "Failed to wait for events." << std::endl;
            break;
        }
        else if (numEvents == 0)
        {
            std::cout << "No events." << std::endl;
            continue;
        }
        for (int i = 0; i < numEvents; ++i)
        {
            int index = -1;
            for (int j = 0; j < 3; ++j)
            {
                if (events[i].data.fd == serverFd[j])
                {
                    index = j;
                    struct sockaddr_in clientAddress;
                    socklen_t clientAddressLength = sizeof(clientAddress);
                    int clientFd = accept(serverFd[j], (struct sockaddr*)&clientAddress, &clientAddressLength);
                    if (clientFd == -1)
                    {
                        std::cerr << "Failed to accept client connection." << std::endl;
                        continue;
                    }
                    event.events = EPOLLIN | EPOLLOUT;
                    event.data.fd = clientFd;
                    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &event) == -1)
                    {
                        std::cerr << "Failed to add client socket to epoll instance." << std::endl;
                        close(clientFd);
                        continue;
                    }
                    client_map.insert(std::pair<int,int>(clientFd,serverFd[i]));
                }
            }
            if (index >= 0)
            {
                if (events[i].events & EPOLLIN)
                {
                    int clientFd = events[i].data.fd;
                    char buffer[1024];
                    int bytesRead = read(clientFd, buffer, sizeof(buffer) - 1);
                    buffer[bytesRead] = '\0';
                    std::cout << "server with port : " << ports[index] << " Received from Client " << clientFd << ": " << bytesRead << std::endl;
                }
                else if (events[i].events & EPOLLOUT)
                {
                    int clientFd = events[i].data.fd;
                }
            }
        }
    }
    for (int i = 0;i < 3;i++)
    {
        close(serverFd[i]);
    }
    close(epollFd);
    return 0;
}
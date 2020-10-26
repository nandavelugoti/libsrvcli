#ifndef LIBSRVCLI_HPP
#define LIBSRVCLI_HPP

#include <thread>
#include "ConnectionConfig.hpp"
#include "SafeQueue.hpp"

class Server
{
    ConnectionConfig config; 
    int sockfd;  // listen on sock_fd
    SafeQueue<int> connectionsBuffer;
    void (*connectionHandler)(int);
    std::thread tConnAcceptWorker, tConnHandleWorker;

public:
    
    Server(void (*)(int), ConnectionConfig);
    ~Server();
    int start();
    void quit();

private:

    void* getInAddr(struct sockaddr*);
    void acceptConnections();
    void handleConnections();
};

class Client 
{
    ConnectionConfig config;
    int sockfd;  // listen on sock_fd
    void (*connectionHandler)(int);

public:

    Client(ConnectionConfig);
    ~Client();
    int connect();
    void disconnect();

private:

    void* getInAddr(struct sockaddr*);
};

#endif

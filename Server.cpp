#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "libsrvcli.hpp"

#define MAX_CONNECTIONS 100
#define BACKLOG_CONNECTIONS 20 

#ifndef DEBUG_PRINTS
#define DEBUG_PRINTS 0
#endif

#if DEBUG_PRINTS
#define perror(msg) perror(msg)
#else
#define perror(msg) 
#endif

using namespace std;

Server::Server(void (*handler)(int), ConnectionConfig config)
{
    this->config = config;
    connectionHandler = handler; 
}

Server::~Server()
{
    quit();
}

int Server::start() {
    int yes = 1, rv;
    struct addrinfo hints, *servinfo, *p; 
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

	if ((rv = getaddrinfo(NULL, config.getPort().c_str(), &hints, &servinfo)) != 0)
    {
        perror(gai_strerror(rv));
        return -1;
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
    {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
			perror("setsockopt");
            return -1;
        }

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
			close(sockfd);
			perror("server: bind");
            continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)
    {
		perror("server bind");
        return -1;
    }

	if (listen(sockfd, BACKLOG_CONNECTIONS) == -1)
    {
		perror("listen");
        return -1;
    }


    tConnAcceptWorker = thread(&Server::acceptConnections, this);

    tConnHandleWorker = thread(&Server::handleConnections, this);

    return 0;
}

void Server::quit()
{
    close(sockfd);
    tConnAcceptWorker.join();
    tConnHandleWorker.join();
    cout << "Bye!" << endl;
}

void* Server::getInAddr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void Server::acceptConnections()
{
    struct sockaddr_storage their_addr;
    socklen_t sin_size = sizeof their_addr;
    char s[INET6_ADDRSTRLEN];
    cout << "SOCK fd: " << sockfd << endl;
    while (true)
    {
        if(!(connectionsBuffer.size() < MAX_CONNECTIONS))
            continue;

        int newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

        if (newfd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family, getInAddr((struct sockaddr *)&their_addr), s, sizeof s);

#if DEBUG_PRINTS
        cout << "server: got connection from" << s << endl;
#endif

        connectionsBuffer.enqueue(newfd);
    }
}

void Server::handleConnections()
{
    while (true)
    {
        if(connectionsBuffer.empty())
            continue;
        int connfd = connectionsBuffer.dequeue();
        thread t(connectionHandler, connfd);
        t.detach();
    }
}

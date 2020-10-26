#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <cstring>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "libsrvcli.hpp"

#ifndef DEBUG_PRINTS
#define DEBUG_PRINTS 0
#endif

#if DEBUG_PRINTS
#define perror(msg) perror(msg)
#else
#define perror(msg) 
#endif

using namespace std;

Client::Client(ConnectionConfig config)
{
    this->config = config;
}

Client::~Client()
{
    disconnect();
}

int Client::connect()
{
    int yes = 1, rv;
    struct addrinfo hints, *servinfo, *p; 
    char s[INET6_ADDRSTRLEN];
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

	if ((rv = getaddrinfo(config.getIP().c_str(), config.getPort().c_str(), &hints, &servinfo)) != 0)
    {
        perror(gai_strerror(rv));
        return -1;
	}

    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }

    if (p == NULL)
    {
        perror("client: failed to connect");
        return -1;
    }

    inet_ntop(p->ai_family, getInAddr((struct sockaddr *)p->ai_addr), s, sizeof s);
    
#if DEBUG_PRINTS
    cout << "client: establishing connectioj to " << s << endl;
#endif

	freeaddrinfo(servinfo); // all done with this structure

    return sockfd;
}

void Client::disconnect()
{
    close(sockfd);
}

void* Client::getInAddr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

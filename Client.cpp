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

using namespace std;

Client::Client(void (*handler)(int), ConnectionConfig config)
{
    this->config = config;
    connectionHandler = handler; 
}

Client::~Client()
{
    disconnect();
}

int Client::establishConnection()
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
        cerr << "getaddrinfo:" << gai_strerror(rv) << endl;
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
        cerr << "client: failed to connect" << endl;
        return -1;
    }

    inet_ntop(p->ai_family, getInAddr((struct sockaddr *)p->ai_addr), s, sizeof s);
    
    cout << "client: establishing connection to " << s << endl;

	freeaddrinfo(servinfo); // all done with this structure

    connectionHandler(sockfd);
    
    return 0;
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

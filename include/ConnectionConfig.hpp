#ifndef CONNECTIONCONFIG_HPP
#define CONNECTIONCONFIG_HPP

class ConnectionConfig
{
    std::string port;
    std::string ip;
    std::string protocol;

public:

    ConnectionConfig() {}

    ConnectionConfig(std::string ip, std::string port, std::string protocol)
    {
        this->ip = ip;
        this->port = port;
        this->protocol = protocol;
    }

    ~ConnectionConfig() {}

    std::string getIP()
    {
        return ip;
    }

    std::string getPort()
    {
        return port;
    }

    std::string getProtocol()
    {
        return protocol;
    }
};

#endif

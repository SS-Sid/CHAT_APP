#ifndef CONNECTION_H
#define CONNECTION_H


#include <iostream>
#include <netinet/in.h>     //  sockaddr_in


class Connection
{
private:
    void init_variables();

public:
	std::string m_name;
    struct sockaddr_in m_address;
    int m_fd;

    Connection();
};


#endif
#ifndef LINKED_LIST_H
#define LINKED_LIST_H


#include "connection.hpp"
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>

#define MAX_CLIENTS 3

class Node
{
private:
    void init_variables();

public:
    Node();

    Connection * connection_ptr;
    struct Node * next;
};

//-------------------------------------------------

class Linked_list
{
private:
    int num_clients;
    Node * head;

    void init_variables();

public:
    Linked_list();
    
    bool is_empty();
    bool is_full();

    void push_back(Connection *);
    Connection * del_connection(Connection *);

    std::string get_names();
    void print();
    Connection * find_name(std::string);

    void broadcast_msg(Connection * p_client, std::string msg);
    void p2p_msg(Connection * p_client, std::string msg);
};


#endif
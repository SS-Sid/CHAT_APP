#ifndef SERVER_H
#define SERVER_H


#include "linked_list.hpp"
#include <pthread.h>
#include <strings.h>
#include <filesystem>

class Server
{
private:
    int m_port;
    int m_listen_fd;
    struct sockaddr_in m_server;
    
    Linked_list * m_connections_list;
    bool is_active;

    bool is_ftp_server;

    pthread_t m_thread;
    struct sockaddr_in m_client;

    void handle_uploader(Connection * p_client);
    void ftp_server_handler(Connection * p_client);


    void init_variables(char * argv[]);
    void setup_server();

    int set_username(Connection * p_client);
    int greet_client(Connection * p_client);


    void broadcast_request(Connection * p_client, std::string p_user_input);
    void p2p_request(Connection * p_client, std::string p_user_input);

    void handle_downloader(Connection * p_client);
    void server_request(Connection * p_client, std::string p_user_input);
    bool handle_client_request(Connection * p_client);
    void * manage_clients(void * p_client);

    void remove_client(Connection * p_client);

    static void * thread_creation(void * args);
    typedef struct thread_creation_args
    {
        Server* server;
        void* true_arg;
        thread_creation_args(Server * s, void * ta) : server(s), true_arg(ta){}
    }tca;


public:
    Server(char * argv[]);

    bool is_online();
    void manage_server();
    void end_server();
};


#endif
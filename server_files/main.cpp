#include "server.hpp"

int main(int argc, char * argv[])
{
    Server * server = new Server(argv);

    while(server->is_online())
    {
        server->manage_server();
    }

    server->end_server();
    
    return 0;
}

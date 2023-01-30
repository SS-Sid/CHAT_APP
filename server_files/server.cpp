#include "server.hpp"

#define BACKLOG_SIZE 1
#define BUFFER_SIZE 4096
#define NAME_LIMIT 10

void Server :: init_variables(char * argv[])
{
    this->m_port = atoi(argv[1]);
    this->m_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    this->m_server.sin_family = AF_INET;
    this->m_server.sin_addr.s_addr = INADDR_ANY;
    this->m_server.sin_port = htons(this->m_port);

    this->m_connections_list = new Linked_list();
    this->is_active = false;

    this->is_ftp_server = atoi(argv[2]);
}

void Server :: setup_server()
{
    int opt = 1;
    setsockopt(this->m_listen_fd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char *) &opt, sizeof(opt));
    bind(this->m_listen_fd, (struct sockaddr *) &this->m_server, sizeof(this->m_server));
    listen(this->m_listen_fd, BACKLOG_SIZE);
    this->is_active = true;

    std::cout << "[DEBUG] Server ONLINE." << std::endl;
}

Server :: Server(char * argv[])
{
    init_variables(argv);
    setup_server();
}

bool Server :: is_online()
{
    return this->is_active;
}

int Server :: set_username(Connection * p_client)
{
    //std::string user_input_msg = "[SERVER] Enter username:: \n";
    
    char * user_input = new char[NAME_LIMIT+1];
    
    /*std::string name_length_exceeded_msg = "[SERVER] Name limit is ";
    name_length_exceeded_msg += std::to_string(NAME_LIMIT);
    name_length_exceeded_msg += " characters only!!\n";*/

    std::string is_accepted_msg = "0";//"[SERVER] This username already exists!!\n";

    bool name_set = false;

    while(name_set == false)
    {
        //write(p_client->m_fd, user_input_msg.c_str(), user_input_msg.length());
        bzero(user_input, NAME_LIMIT+1);
        int input_length = read(p_client->m_fd, user_input, NAME_LIMIT+1);

        std::cout << "[INPUT_LENGTH]" << input_length << std::endl;
        if (input_length < 0)
        {
            return -1;
        }
        /*else if (input_length > NAME_LIMIT)
        {
            write(p_client->m_fd, name_length_exceeded_msg.c_str(), name_length_exceeded_msg.length());

            while (input_length > NAME_LIMIT)
            {
                std::cout << "[1]" << user_input << "||" << (user_input[NAME_LIMIT] == '\0') << "||" << std::endl;
                bzero(user_input, NAME_LIMIT+1);
                input_length = read(p_client->m_fd, user_input, NAME_LIMIT+1);
                std::cout << "[INPUT_LENGTH]" << input_length << "||" << std::endl;
                std::cout << "[2]" << user_input << "||" << (user_input[input_length] == EOF) << "||" << std::endl;
            }
        }*/
        else if (this->m_connections_list->find_name(user_input) != nullptr)
        {
            write(p_client->m_fd, is_accepted_msg.c_str(), is_accepted_msg.length());
        }
        else
        {
            p_client->m_name = user_input;
            name_set = true;
        }
    }
    is_accepted_msg = "1";
    write(p_client->m_fd, is_accepted_msg.c_str(), is_accepted_msg.length());

    std::cout << "[DEBUG] Username INITIALIZED." << std::endl;
    return 0;
}

int Server :: greet_client(Connection * p_client)
{
    /*std::string welcome_msg = "[SERVER] Welcome to the server.\n";
    write(p_client->m_fd, welcome_msg.c_str(), welcome_msg.length());*/
    
    if(set_username(p_client) < 0)
    {
        return -1;
    };
    
    std::string joined_msg = "Joined the chat...\n";
    broadcast_request(p_client, joined_msg);

    return 0;
}


void Server :: broadcast_request(Connection * p_client, std::string p_user_input)
{
    std::string output_msg = p_client->m_name;
    output_msg += ":";
    output_msg += p_user_input;
    //output_msg += "\n";

    this->m_connections_list->broadcast_msg(p_client, output_msg);
}

void Server :: p2p_request(Connection * p_client, std::string p_user_input)
{
    this->m_connections_list->p2p_msg(p_client, p_user_input);
}




std::string server_request_commands[] = {
    "/help",
    "/client_list",
};

void Server :: server_request(Connection * p_client, std::string p_user_input)
{
    std::string output_msg = "[SERVER] ";
    if (p_user_input == "/client_list")
    {
        output_msg += "Showing list of clients.\n";
        output_msg += this->m_connections_list->get_names();
    }
    else if (p_user_input == "/help")
    {
        output_msg += "Showing all commands.\n";
        for(std::string &i : server_request_commands)
        {
            output_msg += "\t";
            output_msg += i;
            output_msg += "\n";
        }
    }
    else
    {
        output_msg = "Invalid command.\n";
    }

    int server_response = write(p_client->m_fd, output_msg.c_str(), output_msg.length());
    return;
}

bool Server :: handle_client_request(Connection * p_client)
{
    bool return_value = false;
    char * user_input = new char[BUFFER_SIZE];
    bzero(user_input, BUFFER_SIZE);
    int input_length = read(p_client->m_fd, user_input, BUFFER_SIZE);

    if (input_length > 0)
    {
        switch (user_input[0])
        {
            case '@':
                std::cout << "[DEBUG] p2p request RECEIVED." << std::endl;
                p2p_request(p_client, user_input);

                break;
                
            case '/':
                std::cout << "[DEBUG] server request RECEIVED." << std::endl;
                server_request(p_client, user_input);

                break;
            
            default:
                std::cout << "[DEBUG] broadcast request RECEIVED." << std::endl;
                broadcast_request(p_client, user_input);

                break;
        }
        return_value = true;
    }
    delete user_input;
    return return_value;
}

void Server :: remove_client(Connection * p_client)
{
    std::string departing_msg = "Left the chat.\n";
    broadcast_request(p_client, departing_msg);
    
    Connection * removed_client = this->m_connections_list->del_connection(p_client);
    close(removed_client->m_fd);
    delete removed_client;

    std::cout << "[DEBUG] client REMOVED." << std::endl;
    pthread_detach(pthread_self());
}



void Server :: handle_downloader(Connection * p_client)
{
    char * user_input = new char[BUFFER_SIZE];
    bzero(user_input, BUFFER_SIZE);
    int input_length = read(p_client->m_fd, user_input, BUFFER_SIZE);
    std::string user_str = "./database/";
    user_str += user_input;
    const char * fname = user_str.c_str();
    std::cout << fname << std::endl;


    FILE * fpath = fopen(fname, "rb");
    std::cout << fpath << std::endl;
    if (fpath == NULL)
    {
        std::string no_file_msg = "0";
        std::cout << no_file_msg << std::endl;
        int server_response = write(p_client->m_fd, no_file_msg.c_str(), no_file_msg.length());
        return;
    }
    else
    {
        std::string yes_file_msg = "1";
        std::cout << yes_file_msg << std::endl;
        int server_response = write(p_client->m_fd, yes_file_msg.c_str(), yes_file_msg.length());
    }

    int fsize = std::filesystem::file_size(fname);
    std::string fsize_msg = std::to_string(fsize);
    std::cout << fsize_msg << std::endl;
    int server_response = write(p_client->m_fd, fsize_msg.c_str(), fsize_msg.length());



    char *file_data = new char[BUFFER_SIZE];
    size_t nbytes = 0;
    int sent;
    while ( (nbytes = fread(file_data, BUFFER_SIZE, 1, fpath)) > 0)
    {
        sent = send(p_client->m_fd, file_data, nbytes, 0);
    }

    return;
}


void Server :: handle_uploader(Connection * p_client)
{
    char * user_input = new char[BUFFER_SIZE];
    bzero(user_input, BUFFER_SIZE);
    int input_length = read(p_client->m_fd, user_input, BUFFER_SIZE);
    
    std::string user_str = user_input;
    int str_sep = user_str.find(' ');
    
    std::string fname_str = "./database/";
    fname_str += user_str.substr(0, str_sep);
    const char* fname = fname_str.c_str();
    std::cout << fname << std::endl;
    
    int fsize = atoi(user_str.substr(str_sep + 1).c_str());
    std::cout << fsize << std::endl;
    
    int n;
    FILE *fpath;
    char * buffer = new char[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);

    fpath = fopen(fname, "wb+");
    std::cout << fpath << std::endl;
    int total = 0;
    while (1) {
        n = recv(p_client->m_fd, buffer, BUFFER_SIZE, 0);
        
        if (n <= 0){
            break;
            return;
        }
        
        std::cout << buffer << std::endl;
        
        total += n;
        fwrite(buffer, n, 1, fpath);
        bzero(buffer, BUFFER_SIZE);
    }
    std::cout << total << std::endl;
    std::string output_msg = (total == fsize) ? "1" : "0";
    std::cout << output_msg << std::endl;
    int server_response = write(p_client->m_fd, output_msg.c_str(), output_msg.length());
    return;
}


void Server :: ftp_server_handler(Connection * p_client)
{
    char * user_input = new char[BUFFER_SIZE];
    bzero(user_input, BUFFER_SIZE);
    int input_length = read(p_client->m_fd, user_input, BUFFER_SIZE);

    if (input_length > 0)
    {
        std::string user_str = user_input;
        std::cout << "|" << user_str << "|" << std::endl;
        if(user_str.compare("/upload") == 0)
        {
            handle_uploader(p_client);
        }
        else if(user_str.compare("/download") == 0)
        {
            handle_downloader(p_client);
        }
        else
        {
            std::cout << "[-] NO REQUEST TAKEN" << std::endl;
        }
    }
    delete user_input;
    return;
}


void * Server :: manage_clients(void * p_client)
{
    Connection * local_client = (Connection *) p_client;

    if (this->is_ftp_server)
    {
        ftp_server_handler(local_client);
    }

    else
    {
        if (greet_client(local_client) < 0)
        {
            remove_client(local_client);
            return p_client;
        }

        while(handle_client_request(local_client));
    }

    remove_client(local_client);

    return p_client;
}

void * Server :: thread_creation(void * args)
{
    tca* thread_args = static_cast<tca *>(args);
    Server* server_t = thread_args->server;
    void * true_arg_t = thread_args->true_arg;

    void * result = server_t->manage_clients(true_arg_t);
    delete thread_args;
    return result; 
}

void Server :: manage_server()
{
    socklen_t client_size = sizeof(this->m_client);
    int client_fd = accept(this->m_listen_fd, (struct sockaddr *) &this->m_client, &client_size);

    std::cout << "[DEBUG] Client JOIN request RECEIVED." << std::endl;
    if (m_connections_list->is_full())
    {
        close(client_fd);
        std::cout << "[DEBUG] Client REJECTED." << std::endl;
        return;
    }

    Connection * current_client = new Connection();
    current_client->m_address = this->m_client;
    current_client->m_fd = client_fd;

    this->m_connections_list->push_back(current_client);

    std::cout << "[DEBUG] Client ACCEPTED." << std::endl;
    pthread_create(&this->m_thread, NULL, &Server::thread_creation, new tca(this, (void *) current_client));
}

void Server :: end_server()
{
    close(this->m_listen_fd);

    std::cout << "[DEBUG] Server CLOSED." << std::endl;
    return;
}
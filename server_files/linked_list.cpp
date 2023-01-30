#include "linked_list.hpp"

void Node :: init_variables()
{
    this->connection_ptr = nullptr;
    this->next = nullptr;
}


Node :: Node()
{
    init_variables();
}

//--------------------------------------------------------


void Linked_list :: init_variables()
{
    this->num_clients = 0;
    this->head = nullptr;
}


Linked_list :: Linked_list()
{
    init_variables();
}


bool Linked_list :: is_empty()
{
    return (this->num_clients == 0);
}


bool Linked_list :: is_full()
{
    return (this->num_clients == MAX_CLIENTS);
}


void Linked_list :: push_back(Connection * p_connection_ptr)
{
    if (is_full())
    {
        return;
    }

    if (is_empty())
    {
        this->head = new Node();
        ++this->num_clients;
        this->head->connection_ptr = p_connection_ptr;
        
        return;
    }

    Node * temp = this->head;
    while (temp->next)
    {
        temp = temp->next;
    }
    
    Node * new_node = new Node();
    ++this->num_clients;
    new_node->connection_ptr = p_connection_ptr;
    temp->next = new_node;

    return;
}


Connection * Linked_list :: del_connection(Connection * p_connection)
{
    if (is_empty())
    {
        return NULL;
    }

    if (p_connection == this->head->connection_ptr)
    {
        Connection * delnode_connection_ptr = this->head->connection_ptr;
        
        Node * node_to_be_del = this->head;
        this->head = this->head->next;
        
        delete node_to_be_del;
        --this->num_clients;
        
        return delnode_connection_ptr;
    }

    Node * temp = this->head;
    while (temp->next->connection_ptr != p_connection)
    {
        temp = temp->next;
    }

    Node * delnode = temp->next;
    Connection * delnode_connection_ptr = temp->next->connection_ptr;
    temp->next = temp->next->next;

    delete delnode;
    --this->num_clients;
    
    return delnode_connection_ptr;
} 


std::string Linked_list :: get_names()
{
    std::string client_array = "";
    
    Node * temp = this->head;
    for (int i = 0; i < this->num_clients; i++)
    {
        client_array += temp->connection_ptr->m_name;
        client_array += "\n";
        temp = temp->next;
    }

    return client_array;
}


void Linked_list :: print()
{
    Node * temp = this->head;
    for (int i = 0; i < this->num_clients; i++)
    {
        std::cout << temp->connection_ptr->m_name << std::endl;
        temp = temp->next;
    }

    return;
}


Connection * Linked_list :: find_name(std::string p_name)
{
	if (this->head)
	{
		Node * temp = this->head;
		while (temp)
		{
            std::cout << "[FIND_NAME]" << temp->connection_ptr->m_name << "||" << p_name << "||" << std::endl;
			if (temp->connection_ptr->m_name == p_name)
			{
				return temp->connection_ptr;
			}
			temp = temp->next;
		}
	}
	return nullptr;
}

void Linked_list :: broadcast_msg(Connection * p_client, std::string msg)
{
    int server_response;
    Node * temp = this->head;
    while (temp != nullptr)
    {
        std::cout << "[BROADCAST]" << temp->connection_ptr->m_name << "||" << std::endl;
        if (temp->connection_ptr != p_client)
        {
            server_response = write(temp->connection_ptr->m_fd, msg.c_str(), msg.length());
            std::cout << "broadcast sent to " << temp->connection_ptr->m_name << std::endl;
        }
             
        temp = temp->next;
    }
}

void Linked_list :: p2p_msg(Connection * p_client, std::string msg)
{
    int server_response;

    int str_sep = msg.find(' ');
    std::string receiver_name = msg.substr(1, str_sep - 1);
    std::string output_msg = p_client->m_name;
    output_msg += " (whispered) ";
    output_msg += msg.substr(str_sep + 1);
    output_msg += "\n";

    std::cout << "[P2P]MSG,RECEIVER||" << output_msg << "||" << receiver_name << std::endl;
    Connection * receiver_client = find_name(receiver_name);
    if (receiver_client != nullptr)
    {
        std::cout << "[P2P]RECEIVER||" << receiver_client->m_name << std::endl;
        server_response = write(receiver_client->m_fd, output_msg.c_str(), output_msg.length());
        std::cout << "p2p sent" << std::endl;
    }
}


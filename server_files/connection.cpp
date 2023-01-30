#include "connection.hpp"


void Connection :: init_variables()
{
    this->m_name = "New_User";
}

Connection :: Connection()
{
    init_variables();

    std::cout << "[CONNECTION] new connection established." << std::endl;
}

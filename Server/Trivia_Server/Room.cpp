#include "Room.h"

void Room::addUser(LoggedUser user)
{
	m_users.insert(user);
}

void Room::removeUser(LoggedUser user)
{
	m_users.erase(user);
}

std::set<std::string> Room::getAllUsers()
{
    std::set<std::string> usernames;

    for (const auto& user : m_users) 
    {
        usernames.insert(user.getUserName());
    }

    return usernames;
}


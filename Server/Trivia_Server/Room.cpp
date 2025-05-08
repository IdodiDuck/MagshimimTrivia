#include "Room.h"

Room::Room(const RoomData& roomData) : m_metadata(roomData), m_users()
{
}

Room::~Room()
{
    m_users.clear();
}

void Room::addUser(const LoggedUser& user)
{
	m_users.insert(user);
}

void Room::removeUser(const LoggedUser& user)
{
	m_users.erase(user);
}

std::set<std::string> Room::getAllUsers() const
{
    std::set<std::string> usernames;

    for (const auto& user : m_users) 
    {
        usernames.insert(user.getUserName());
    }

    return usernames;
}

RoomData Room::getRoomData() const
{
    return m_metadata;
}
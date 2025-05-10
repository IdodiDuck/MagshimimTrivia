#include "Room.h"

Room::Room(const RoomData& roomData) : m_metadata(roomData), m_users()
{
}

void Room::addUser(const LoggedUser& user)
{
	m_users.insert(user);
}

void Room::removeUser(const LoggedUser& user)
{
	m_users.erase(user);
}

void Room::changeRoomStatus(const RoomStatus& newStatus)
{
    m_metadata.status = newStatus;
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
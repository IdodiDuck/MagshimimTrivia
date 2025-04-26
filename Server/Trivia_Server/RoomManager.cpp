#include "RoomManager.h"

#include <stdexcept>

void RoomManager::createRoom(const LoggedUser& user, const RoomData& data)
{
    if (data.id <= 0) 
    {
        throw std::invalid_argument("Error: Invalid room ID");
    }

    if (m_rooms.find(data.id) != m_rooms.cend()) 
    {
        throw std::runtime_error("Error: Room with ID " + std::to_string(data.id) + " already exists");
    }

    Room newRoom(data);
    newRoom.addUser(user);

    m_rooms.emplace(data.id, std::move(newRoom));
}

void RoomManager::deleteRoom(int ID)
{
    auto it = m_rooms.find(ID);
    if (it == m_rooms.cend()) 
    {
        throw std::runtime_error("Error: Room with ID " + std::to_string(ID) + " not found");
    }

    m_rooms.erase(it);
}

RoomStatus RoomManager::getRoomState(int ID) const
{
    auto it = m_rooms.find(ID);
    if (it == m_rooms.cend()) 
    {
        throw std::runtime_error("Error: Room with ID " + std::to_string(ID) + " not found");
    }

    return it->second.getRoomData().status;
}

std::vector<RoomData> RoomManager::getRooms() const
{
    std::vector<RoomData> rooms;

    for (const auto& [id, room] : m_rooms) 
    {
        rooms.push_back(room.getRoomData());
    }

    return rooms;
}

std::optional<const Room&> RoomManager::getRoom(int ID) const
{
    auto it = m_rooms.find(ID);
    if (it != m_rooms.cend())
    {
        return std::optional<const Room&>(it->second);
    }
    return std::nullopt;
}

#include "RoomManager.h"

#include <stdexcept>

RoomManager::~RoomManager()
{
    this->m_rooms.clear();
}

void RoomManager::createRoom(const LoggedUser& user, const RoomData& data)
{
    const int MIN_VALID_ID = 1;
    if (data.id < MIN_VALID_ID)
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
    auto roomIt = m_rooms.find(ID);
    if (roomIt == m_rooms.cend())
    {
        throw std::runtime_error("Error: Room with ID " + std::to_string(ID) + " not found");
    }

    m_rooms.erase(roomIt);
}

RoomStatus RoomManager::getRoomState(const int ID) const
{
    auto roomIt = m_rooms.find(ID);
    if (roomIt == m_rooms.cend())
    {
        throw std::runtime_error("Error: Room with ID " + std::to_string(ID) + " not found");
    }

    return roomIt->second.getRoomData().status;
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

std::optional<std::reference_wrapper<const Room>> RoomManager::getRoom(const int ID) const
{
    auto roomIt = m_rooms.find(ID);
    if (roomIt != m_rooms.end())
    {
        return std::cref(roomIt->second);
    }

    return std::nullopt;
}

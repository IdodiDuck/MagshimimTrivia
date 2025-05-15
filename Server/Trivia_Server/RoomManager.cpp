#include "RoomManager.h"

RoomManager::~RoomManager()
{
    this->m_rooms.clear();
}

void RoomManager::createRoom(const LoggedUser& user, const RoomData& data)
{
    std::unique_lock lock(m_roomsMutex);

    const int MIN_VALID_ID = 1;

    if (data.id < MIN_VALID_ID)
    {
        throw std::invalid_argument("Error: Invalid room ID");
    }

    if (doesRoomExist(data.id))
    {
        throw std::runtime_error("Error: Room with ID " + std::to_string(data.id) + " already exists");
    }

    Room newRoom(data);
    newRoom.addUser(user);

    m_rooms.emplace(data.id, std::move(newRoom));
}

void RoomManager::deleteRoom(const int ID)
{
    std::unique_lock lock(m_roomsMutex);

    auto roomIt = m_rooms.find(ID);
    if (!doesRoomExist(ID))
    {
        throw std::runtime_error("Error: Room with ID " + std::to_string(ID) + " not found");
    }

    roomIt->second.changeRoomStatus(RoomStatus::CLOSED);
    m_rooms.erase(roomIt);
}

void RoomManager::removeUserFromRoom(const int ID, const LoggedUser& removedUser)
{
    auto desiredRoom = this->getRoom(ID);

    if (desiredRoom.has_value())
    {
        desiredRoom.value().removeUser(removedUser);
    }
}

RoomStatus RoomManager::getRoomState(const int ID) const
{
    if (!doesRoomExist(ID))
    {
        throw std::runtime_error("Error: Room with ID " + std::to_string(ID) + " not found");
    }

    return this->m_rooms.at(ID).getRoomData().status;
}

std::vector<RoomData> RoomManager::getRooms() const
{
    std::shared_lock lock(m_roomsMutex);

    std::vector<RoomData> rooms;
    for (const auto& [id, room] : m_rooms)
    {
        rooms.push_back(room.getRoomData());
    }

    return rooms;
}

std::optional<Room> RoomManager::getRoom(const int ID) const
{
    std::shared_lock lock(m_roomsMutex);

    if (doesRoomExist(ID))
    {
        return m_rooms.at(ID);
    }

    return std::nullopt;
}

bool RoomManager::doesRoomExist(const int ID) const
{
    std::shared_lock lock(m_roomsMutex);
    return (this->m_rooms.find(ID) != this->m_rooms.cend());
}

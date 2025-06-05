#include "RoomManager.h"

#include "ManagerException.h"

RoomManager::~RoomManager()
{
    this->m_rooms.clear();
}

void RoomManager::createRoom(const LoggedUser& user, const RoomData& data)
{
    std::shared_lock readingLock(m_roomsMutex);

    const int MIN_VALID_ID = 1;

    if (data.id < MIN_VALID_ID)
    {
        throw ManagerException("Error: Invalid room ID!");
    }

    if (m_rooms.find(data.id) != m_rooms.cend())
    {
        throw ManagerException("Error: Room with ID " + std::to_string(data.id) + " already exists!");
    }

    readingLock.unlock();

    Room newRoom(data);
    newRoom.addUser(user);

    std::unique_lock writingLock(m_roomsMutex);
    m_rooms.emplace(data.id, std::move(newRoom));
}

void RoomManager::deleteRoom(const int ID)
{
    std::shared_lock readingLock(m_roomsMutex);

    auto roomIt = m_rooms.find(ID);
    if (!doesRoomExist(ID))
    {
        throw ManagerException("Error: Room with ID " + std::to_string(ID) + " not found!");
    }

    readingLock.unlock();

    std::unique_lock writeLock(m_roomsMutex);
    m_rooms.erase(roomIt);
}

void RoomManager::removeUserFromRoom(const int ID, const LoggedUser& removedUser)
{
    auto desiredRoom = this->getRoomReference(ID);

    if (desiredRoom.has_value())
    {
        desiredRoom.value().get().removeUser(removedUser);
    }
}

void RoomManager::addUserToRoom(const int ID, const LoggedUser& addedUser)
{
    auto desiredRoom = this->getRoomReference(ID);

    if (desiredRoom.has_value())
    {
        desiredRoom.value().get().addUser(addedUser);
    }
}

RoomStatus RoomManager::getRoomState(const int ID) const
{
    if (!doesRoomExist(ID))
    {
        throw ManagerException("Error: Room with ID " + std::to_string(ID) + " not found!");
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

std::optional<std::reference_wrapper<Room>> RoomManager::getRoomReference(const int ID)
{
    std::unique_lock lock(m_roomsMutex);

    auto desiredRoomIt = m_rooms.find(ID);

    if (desiredRoomIt != m_rooms.cend())
    {
        return std::optional<std::reference_wrapper<Room>>(desiredRoomIt->second);
    }

    return std::nullopt;
}

bool RoomManager::doesRoomExist(const int ID) const
{
    std::shared_lock lock(m_roomsMutex);
    return (this->m_rooms.find(ID) != this->m_rooms.cend());
}

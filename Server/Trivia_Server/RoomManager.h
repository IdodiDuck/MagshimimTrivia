#pragma once

#include "Room.h"

#include <vector>
#include <optional>
#include <unordered_map>
#include <shared_mutex>

class RoomManager
{

public:
	// C'tor & D'tor - 
	~RoomManager();

	// Setters - 
	void createRoom(const LoggedUser& user, const RoomData& data);
	void deleteRoom(const int ID);
	void removeUserFromRoom(const int ID, const LoggedUser& removedUser);
	void addUserToRoom(const int ID, const LoggedUser& addedUser);

	// Getters - 
	RoomStatus getRoomState(const int ID) const;
	std::vector<RoomData> getRooms() const;
	std::optional<Room> getRoom(const int ID) const;
	std::optional<std::reference_wrapper<Room>> getRoomReference(const int ID);

private:
	// Attributes - 
	std::unordered_map<unsigned int, Room> m_rooms;
	mutable std::shared_mutex m_roomsMutex;

	// Support Methods - 
	bool doesRoomExist(const int ID) const;

};

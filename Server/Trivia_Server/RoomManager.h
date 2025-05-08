#pragma once

#include "Room.h"

#include <vector>
#include <optional>
#include <unordered_map>

class RoomManager
{

public:
	// C'tor & D'tor - 
	~RoomManager();

	// Setters - 
	void createRoom(const LoggedUser& user, const RoomData& data);
	void deleteRoom(const int ID);

	// Getters - 
	RoomStatus getRoomState(const int ID) const;
	std::vector<RoomData> getRooms() const;
	std::optional<std::reference_wrapper<const Room>> getRoom(const int ID) const;

private:
	// Attributes - 
	std::unordered_map<unsigned int, Room> m_rooms;

};

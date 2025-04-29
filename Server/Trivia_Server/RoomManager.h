#pragma once

#include "Room.h"

#include <vector>
#include <optional>
#include <unordered_map>

class RoomManager
{

public:

	void createRoom(const LoggedUser& user, const RoomData& data);
	void deleteRoom(const int ID);

	RoomStatus getRoomState(const int ID) const;
	std::vector<RoomData> getRooms() const;
	std::optional<std::reference_wrapper<const Room>> getRoom(const int ID) const;

private:

	std::unordered_map<const unsigned int, Room> m_rooms;

};

#pragma once

#include "Room.h"

#include <vector>
#include <optional>
#include <unordered_map>

class RoomManager
{

public:

	void createRoom(const LoggedUser& user, const RoomData& data);
	void deleteRoom(int ID);

	RoomStatus getRoomState(int ID) const;
	std::vector<RoomData> getRooms() const;
	std::optional<const Room&> getRoom(int ID) const;

private:

	std::unordered_map<const unsigned int, Room> m_rooms;

};

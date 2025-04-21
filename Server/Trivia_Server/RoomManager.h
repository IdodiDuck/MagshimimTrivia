#pragma once

#include "Room.h"

#include <map>
#include <vector>
#include <optional>

class RoomManager
{

public:

	void createRoom(LoggedUser user, RoomData data);
	void deleteRoom(int ID);

	RoomStatus getRoomState(int ID) const;
	std::vector<RoomData> getRooms() const;
	std::optional<const Room&> getRoom(int ID) const;

private:

	std::map<const unsigned int, Room> m_rooms;

};


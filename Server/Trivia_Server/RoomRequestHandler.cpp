#include "RoomRequestHandler.h"

#include "RequestHandlerFactory.h"
#include "JsonResponsePacketSerializer.h"

RoomRequestHandler::RoomRequestHandler(std::weak_ptr<RequestHandlerFactory> handlerFactory, RoomManager& roomManager, const LoggedUser& loggedUser, const Room& usedRoom, const bool isAdmin):
	m_handlerFactory(handlerFactory), m_user(loggedUser), m_roomManager(roomManager), m_room(usedRoom), _isAdmin(isAdmin)
{

}

std::shared_ptr<RequestHandlerFactory> RoomRequestHandler::getFactorySafely()
{
	if (auto factory = m_handlerFactory.lock())
	{
		return factory;
	}

	throw std::runtime_error("RoomRequestHandler: [ERROR]: RequestHandlerFactory is no longer available");
}

Room& RoomRequestHandler::getRoomSafely()
{
	auto roomOptional = this->m_roomManager.getRoomReference(m_room.getRoomData().id);

	if (!roomOptional.has_value())
	{
		throw std::runtime_error("RoomRequestHandler: [ERROR]: Room not found in RoomManager");
	}

	auto roomRef = roomOptional.value();
	return roomRef.get();
}

RequestResult RoomRequestHandler::getRoomState(const RequestInfo& info)
{
    GetRoomStateResponse getRoomStateResponse;

    Room& room = getRoomSafely();
    RoomData usedRoomData = room.getRoomData();

    getRoomStateResponse.status = SUCCESS;
    getRoomStateResponse.hasGameBegun = (usedRoomData.status == RoomStatus::GAME_STARTED);
    getRoomStateResponse.questionsCount = usedRoomData.numOfQuestionsInGame;
    getRoomStateResponse.answerTimeout = usedRoomData.timePerQuestion;
    auto users = room.getAllUsers();
    getRoomStateResponse.players = std::vector<std::string>(users.cbegin(), users.cend());

    RequestResult response;

    if (usedRoomData.status == RoomStatus::GAME_STARTED)
    {
        // TODO AT V4.0.0 -> CREATE GAME HANDLER IF THE GAME HAS STARTED!!!
        response.newHandler = this->_isAdmin ? getFactorySafely()->createRoomAdminRequestHandler(m_user, usedRoomData.id) : getFactorySafely()->createRoomMemberRequestHandler(m_user, usedRoomData.id);
    }

    else if (usedRoomData.status == RoomStatus::CLOSED)
    {
        getRoomStateResponse.status = FAILURE;
        response.newHandler = getFactorySafely()->createMenuRequestHandler(m_user);
    }

    else
    {
        response.newHandler = this->_isAdmin ? getFactorySafely()->createRoomAdminRequestHandler(m_user, usedRoomData.id) : getFactorySafely()->createRoomMemberRequestHandler(m_user, usedRoomData.id);
    }

    response.response = JsonResponsePacketSerializer::serializeResponse(getRoomStateResponse);
    return response;
}

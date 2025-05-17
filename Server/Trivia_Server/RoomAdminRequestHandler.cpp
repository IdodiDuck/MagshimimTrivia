#include "RoomAdminRequestHandler.h"

#include "RequestHandlerFactory.h"
#include "JsonResponsePacketSerializer.h"

RoomAdminRequestHandler::RoomAdminRequestHandler(std::weak_ptr<RequestHandlerFactory> handlerFactory, RoomManager& roomManager, const LoggedUser& loggedUser, const Room& usedRoom):
	RoomRequestHandler(handlerFactory, roomManager, loggedUser, usedRoom, true)
{

}

bool RoomAdminRequestHandler::isRequestRelevant(const RequestInfo& info)
{
	return (info.requestID == static_cast<unsigned int>(RequestCode::CLOSE_ROOM_REQUEST)) ||
		(info.requestID == static_cast<unsigned int>(RequestCode::START_GAME_REQUEST)) ||
		(info.requestID == static_cast<unsigned int>(RequestCode::GET_ROOM_STATE_REQUEST)) ||
		(info.requestID == static_cast<unsigned int>(RequestCode::LEAVE_ROOM_REQUEST));
}

RequestResult RoomAdminRequestHandler::handleRequest(const RequestInfo& requestInfo)
{
	switch (static_cast<RequestCode>(requestInfo.requestID))
	{
		case RequestCode::CLOSE_ROOM_REQUEST:
			return closeRoom(requestInfo);

		case RequestCode::START_GAME_REQUEST:
			return startGame(requestInfo);

		case RequestCode::GET_ROOM_STATE_REQUEST:
			return getRoomState(requestInfo);

		case RequestCode::LEAVE_ROOM_REQUEST:
			return closeRoom(requestInfo);

		default:
			ErrorResponse errorResponse;
			errorResponse.message = "Unknown request type.";
			return
			{
				JsonResponsePacketSerializer::serializeResponse(errorResponse),
				nullptr
			};
	}
}

void RoomAdminRequestHandler::handleDisconnection()
{
	try
	{
		this->m_roomManager.deleteRoom(this->m_room.getRoomData().id);
	}

	catch (...) { }

	this->getFactorySafely()->getLoginManager().logOut(this->m_user.getUserName());
}

RequestResult RoomAdminRequestHandler::closeRoom(const RequestInfo& info)
{
	getRoomSafely().changeRoomStatus(RoomStatus::CLOSED);
	this->m_roomManager.deleteRoom(m_room.getRoomData().id);

	CloseRoomResponse closeRoomResponse = { };
	closeRoomResponse.status = SUCCESS;

	RequestResult response = { };
	response.newHandler = this->getFactorySafely()->createMenuRequestHandler(m_user);
	response.response = JsonResponsePacketSerializer::serializeResponse(closeRoomResponse);

	return response;
}

RequestResult RoomAdminRequestHandler::startGame(const RequestInfo& info)
{
	getRoomSafely().changeRoomStatus(RoomStatus::GAME_STARTED);

	StartGameResponse startGameResponse = { };
	startGameResponse.status = SUCCESS;

	RequestResult response = { };

	// Create here GameRequestHandler instead!!! -> Temporary Handler
	response.newHandler = getFactorySafely()->createRoomAdminRequestHandler(m_user, m_room.getRoomData().id);
	response.response = JsonResponsePacketSerializer::serializeResponse(startGameResponse);

	return response;
}

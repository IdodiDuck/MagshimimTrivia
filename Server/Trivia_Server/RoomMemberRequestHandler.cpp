#include "RoomMemberRequestHandler.h"

#include "RequestHandlerFactory.h"
#include "Constants.h"
#include "JsonResponsePacketSerializer.h"

RoomMemberRequestHandler::RoomMemberRequestHandler(std::weak_ptr<RequestHandlerFactory> handlerFactory, RoomManager& roomManager, const LoggedUser& loggedUser, const Room& usedRoom)
	: m_room(usedRoom), m_handlerFactory(handlerFactory), m_roomManager(roomManager), m_user(loggedUser)
{

}

bool RoomMemberRequestHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
	return (requestInfo.requestID == static_cast<unsigned int>(RequestCode::GET_ROOM_STATE_REQUEST)) ||
		   (requestInfo.requestID == static_cast<unsigned int>(RequestCode::LEAVE_ROOM_REQUEST));
}

RequestResult RoomMemberRequestHandler::handleRequest(const RequestInfo& requestInfo)
{
	switch (static_cast<RequestCode>(requestInfo.requestID))
	{
		case RequestCode::GET_ROOM_STATE_REQUEST:
			return getRoomState(requestInfo);

		case RequestCode::LEAVE_ROOM_REQUEST:
			return leaveRoom(requestInfo);

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

void RoomMemberRequestHandler::handleDisconnection()
{
	this->m_roomManager.removeUserFromRoom(this->m_room.getRoomData().id, this->m_user.getUserName());
	this->getFactorySafely()->getLoginManager().logOut(this->m_user.getUserName());
}

RequestResult RoomMemberRequestHandler::leaveRoom(const RequestInfo& info)
{
	this->m_roomManager.removeUserFromRoom(this->m_room.getRoomData().id, this->m_user);

	LeaveRoomResponse leaveRoomResponse;
	leaveRoomResponse.status = SUCCESS;

	RequestResult response;
	response.newHandler = this->getFactorySafely()->createMenuRequestHandler(m_user);
	response.response = JsonResponsePacketSerializer::serializeResponse(leaveRoomResponse);

	return response;
}

RequestResult RoomMemberRequestHandler::getRoomState(const RequestInfo& info)
{
	GetRoomStateResponse getRoomStateResponse;

	Room& room = getRoomSafely();
	RoomData usedRoomData = room.getRoomData();

	getRoomStateResponse.status = SUCCESS;
	getRoomStateResponse.hasGameBegun = (usedRoomData.status == RoomStatus::GAME_STARTED);
	getRoomStateResponse.questionsCount = usedRoomData.numOfQuestionsInGame;
	getRoomStateResponse.answerTimeout = usedRoomData.timePerQuestion;
	getRoomStateResponse.players = std::vector<std::string>(room.getAllUsers().cbegin(), room.getAllUsers().cend());

	RequestResult response;

	if (getRoomStateResponse.hasGameBegun)
	{
		// Create here GameRequestHandler instead!!! -> Temporary Handler and initialize a new game!!!
		response.newHandler = getFactorySafely()->createRoomMemberRequestHandler(m_user, m_room.getRoomData().id);
	}

	else
	{
		response.newHandler = getFactorySafely()->createRoomMemberRequestHandler(m_user, m_room.getRoomData().id);
	}

	response.response = JsonResponsePacketSerializer::serializeResponse(getRoomStateResponse);
	return response;
}

std::shared_ptr<RequestHandlerFactory> RoomMemberRequestHandler::getFactorySafely()
{
	if (auto factory = m_handlerFactory.lock())
	{
		return factory;
	}

	throw std::runtime_error("RoomMemberRequestHandler: [ERROR]: RequestHandlerFactory is no longer available");
}

Room& RoomMemberRequestHandler::getRoomSafely()
{
	auto roomOptional = this->m_roomManager.getRoomReference(m_room.getRoomData().id);

	if (!roomOptional.has_value())
	{
		throw std::runtime_error("RoomMemberRequestHandler: [ERROR]: Room not found in RoomManager");
	}

	auto roomRef = roomOptional.value();
	return roomRef.get();
}

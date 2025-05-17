#include "RoomMemberRequestHandler.h"

#include "RequestHandlerFactory.h"
#include "Constants.h"
#include "JsonResponsePacketSerializer.h"

RoomMemberRequestHandler::RoomMemberRequestHandler(std::weak_ptr<RequestHandlerFactory> handlerFactory, RoomManager& roomManager, const LoggedUser& loggedUser, const Room& usedRoom):
	RoomRequestHandler(handlerFactory, roomManager, loggedUser, usedRoom, false)
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

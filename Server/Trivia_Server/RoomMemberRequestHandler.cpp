#include "RoomMemberRequestHandler.h"

#include "RequestHandlerFactory.h"
#include "Constants.h"
#include "JsonResponsePacketSerializer.h"

#include "ManagerException.h"

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
	try
	{
		switch (static_cast<RequestCode>(requestInfo.requestID))
		{
			case RequestCode::GET_ROOM_STATE_REQUEST:
				return getRoomState(requestInfo);

			case RequestCode::LEAVE_ROOM_REQUEST:
				return leaveRoom(requestInfo);

			default:
				throw ManagerException("Error: Unknown Type of Request was sent!");
		}
	}
	
	catch (const ManagerException& e)
	{
		ErrorResponse errorResponse = { };
		errorResponse.message = e.what();
		RequestResult requestResult;
		requestResult.response = JsonResponsePacketSerializer::serializeResponse(errorResponse);
		requestResult.newHandler = this->getFactorySafely()->createRoomMemberRequestHandler(this->m_user, this->m_room.getRoomData().id);
		return requestResult;
	}

	catch (const ServerException& e)
	{
		ErrorResponse errorResponse = { };
		errorResponse.message = "Server Error: " + std::string(e.what());
		RequestResult requestResult;
		requestResult.response = JsonResponsePacketSerializer::serializeResponse(errorResponse);
		requestResult.newHandler = this->getFactorySafely()->createLoginRequestHandler();
		return requestResult;
	}
}

void RoomMemberRequestHandler::handleDisconnection()
{
	this->m_roomManager.removeUserFromRoom(this->m_room.getRoomData().id, this->m_user.getUserName());
	this->getFactorySafely()->getLoginManager().logOut(this->m_user.getUserName());
}

RequestResult RoomMemberRequestHandler::leaveRoom(const RequestInfo& info)
{
	try
	{
		this->m_roomManager.removeUserFromRoom(this->m_room.getRoomData().id, this->m_user);
	}

	catch (ManagerException& e) { }

	LeaveRoomResponse leaveRoomResponse;
	leaveRoomResponse.status = SUCCESS;

	RequestResult response;
	response.newHandler = this->getFactorySafely()->createMenuRequestHandler(m_user);
	response.response = JsonResponsePacketSerializer::serializeResponse(leaveRoomResponse);

	return response;
}

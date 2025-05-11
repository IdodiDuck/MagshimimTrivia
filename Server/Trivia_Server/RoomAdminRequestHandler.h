#pragma once

#include "IRequestHandler.h"

#include "RoomManager.h"
#include "Room.h"

class RequestHandlerFactory;

class RoomAdminRequestHandler : public IRequestHandler
{

public:
	// C'tor -
	RoomAdminRequestHandler(std::weak_ptr<RequestHandlerFactory> handlerFactory, RoomManager& roomManager, const LoggedUser& loggedUser, const Room& usedRoom);
	
	// Virtuals - 
	bool isRequestRelevant(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(const RequestInfo& requestInfo) override;

private:
	// Attributes - 
	Room m_room;
	LoggedUser m_user;
	RoomManager& m_roomManager;
	std::weak_ptr<RequestHandlerFactory> m_handlerFactory;

	// Private Methods - 
	RequestResult closeRoom(const RequestInfo& info);
	RequestResult startGame(const RequestInfo& info);
	RequestResult getRoomState(const RequestInfo& info);

	// Support Methods -
	std::shared_ptr<RequestHandlerFactory> getFactorySafely();
};


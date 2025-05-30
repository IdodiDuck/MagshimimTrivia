#pragma once

#include "RoomRequestHandler.h"

class RequestHandlerFactory;

class RoomAdminRequestHandler : public RoomRequestHandler
{

public:
	// C'tor -
	RoomAdminRequestHandler(std::weak_ptr<RequestHandlerFactory> handlerFactory, RoomManager& roomManager, const LoggedUser& loggedUser, const Room& usedRoom);
	
	// Virtuals - 
	bool isRequestRelevant(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(const RequestInfo& requestInfo) override;
	void handleDisconnection() override;

private:
	// Private Methods - 
	RequestResult closeRoom(const RequestInfo& info);
	RequestResult startGame(const RequestInfo& info);
};


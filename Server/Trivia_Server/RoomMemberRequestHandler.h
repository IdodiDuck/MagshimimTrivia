#pragma once

#include "RoomRequestHandler.h"

class RequestHandlerFactory;

class RoomMemberRequestHandler : public RoomRequestHandler
{

public:
	// C'tor -
	RoomMemberRequestHandler(std::weak_ptr<RequestHandlerFactory> handlerFactory, RoomManager& roomManager, const LoggedUser& loggedUser, const Room& usedRoom);

	// Virtuals - 
	bool isRequestRelevant(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(const RequestInfo& requestInfo) override;
	void handleDisconnection() override;

private:
	// Private Methods - 
	RequestResult leaveRoom(const RequestInfo& info);
};


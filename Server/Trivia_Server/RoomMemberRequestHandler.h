#pragma once
#include "IRequestHandler.h"

#include "RoomManager.h"

class RequestHandlerFactory;

class RoomMemberRequestHandler : public IRequestHandler
{
public:
	// C'tor -
	RoomMemberRequestHandler(std::weak_ptr<RequestHandlerFactory> handlerFactory, RoomManager& roomManager, const LoggedUser& loggedUser, const Room& usedRoom);

	// Virtuals - 
	bool isRequestRelevant(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(const RequestInfo& requestInfo) override;
	void handleDisconnection() override;

private:
	// Attributes - 
	Room m_room;
	LoggedUser m_user;
	RoomManager& m_roomManager;
	std::weak_ptr<RequestHandlerFactory> m_handlerFactory;

	// Private Methods - 
	RequestResult leaveRoom(const RequestInfo& info);
	RequestResult getRoomState(const RequestInfo& info);

	// Support Methods -
	std::shared_ptr<RequestHandlerFactory> getFactorySafely();
};


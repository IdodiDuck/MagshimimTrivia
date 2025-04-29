#pragma once

#include "IRequestHandler.h"
#include "LoggedUser.h"

class RequestHandlerFactory;

class MenuRequestHandler : public IRequestHandler
{

public:
	// C'tor & D'tor - 
	MenuRequestHandler(std::weak_ptr<RequestHandlerFactory> handlerFactory, const LoggedUser& user);
	virtual ~MenuRequestHandler();

	// Virtuals - 
	bool isRequestRelevant(const RequestInfo& info) override;
	RequestResult handleRequest(const RequestInfo& info) override;

private:
	// Attributes - 
	LoggedUser m_user;
	std::weak_ptr<RequestHandlerFactory> m_handlerFactory;

	// Room Operation Methods - 
	RequestResult signout(const RequestInfo& info);
	RequestResult getRooms(const RequestInfo& info);
	RequestResult getPlayersInRoom(const RequestInfo& info);
	RequestResult getPersonalStats(const RequestInfo& info);
	RequestResult getHighScore(const RequestInfo& info);
	RequestResult joinRoom(const RequestInfo& info);
	RequestResult createRoom(const RequestInfo& info);

	// Support Methods - 
	std::shared_ptr<RequestHandlerFactory> getFactorySafely();

};

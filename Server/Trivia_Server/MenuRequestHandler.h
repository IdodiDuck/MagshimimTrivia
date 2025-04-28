#pragma once

#include "MenuRequestHandler.h"

#include "IRequestHandler.h"
#include "RequestHandlerFactory.h"
#include "LoggedUser.h"

class MenuRequestHandler : public IRequestHandler
{

public:

	MenuRequestHandler(const LoggedUser& user, RequestHandlerFactory& handlerFactory);
	virtual ~MenuRequestHandler();

	bool isRequestRelevant(const RequestInfo& info) override;
	RequestResult handleRequest(const RequestInfo& info) override;

private:

	LoggedUser m_user;
	RequestHandlerFactory& m_handlerFactory;

	RequestResult signout(const RequestInfo& info);
	RequestResult getRooms(const RequestInfo& info);
	RequestResult getPlayersInRoom(const RequestInfo& info);
	RequestResult getPersonalStats(const RequestInfo& info);
	RequestResult getHighScore(const RequestInfo& info);
	RequestResult joinRoom(const RequestInfo& info);
	RequestResult createRoom(const RequestInfo& info);

};

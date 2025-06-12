#pragma once

#include "IRequestHandler.h"
#include "LoggedUser.h"

#include <atomic>
#include <mutex>

class RequestHandlerFactory;

class MenuRequestHandler : public IRequestHandler
{

public:
	// C'tor & D'tor - 
	MenuRequestHandler(std::weak_ptr<RequestHandlerFactory> handlerFactory, const LoggedUser& user);

	// Virtuals - 
	bool isRequestRelevant(const RequestInfo& info) override;
	RequestResult handleRequest(const RequestInfo& info) override;
	void handleDisconnection() override;

private:
	// Attributes - 
	LoggedUser m_user;
	std::weak_ptr<RequestHandlerFactory> m_handlerFactory;

	// Statics - 
	static std::atomic<unsigned int> m_currentRoomId;

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
	
	unsigned int generateNewRoomId();

};

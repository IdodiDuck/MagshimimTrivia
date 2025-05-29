#pragma once

#include "IRequestHandler.h"

#include "LoggedUser.h"
#include "Game.h"
#include "GameManager.h"

class RequestHandlerFactory;

class GameRequestHandler : public IRequestHandler
{

public:
	// C'tor 
	GameRequestHandler(std::weak_ptr<RequestHandlerFactory> handlerFactory, const LoggedUser& user, GameManager& gameManager, Game& game);
	
	// Virtuals -
	bool isRequestRelevant(const RequestInfo& requestInfo) override;
	RequestResult handleRequest(const RequestInfo& requestInfo) override;
	void handleDisconnection() override;

private:
	// Attributes - 
	std::weak_ptr<RequestHandlerFactory> m_handlerFactory;
	LoggedUser m_user;
	Game& m_game;
	GameManager& m_gameManager;

	// Private Methods - 
	RequestResult getQuestion(const RequestInfo& info);
	RequestResult submitAnswer(const RequestInfo& info);
	RequestResult getGameResults(const RequestInfo& info);
	RequestResult leaveGame(const RequestInfo& info);

	// Support Methods - 
	std::shared_ptr<RequestHandlerFactory> getFactorySafely();

};


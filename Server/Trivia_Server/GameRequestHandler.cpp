#include "GameRequestHandler.h"

#include "RequestHandlerFactory.h"
#include "JsonResponsePacketSerializer.h"

#include "ManagerException.h"

GameRequestHandler::GameRequestHandler(std::weak_ptr<RequestHandlerFactory> handlerFactory, const LoggedUser& user):
	m_handlerFactory(handlerFactory), m_user(user) //TODO: Add the rest of attributes to Initialization-Line
{

}

bool GameRequestHandler::isRequestRelevant(const RequestInfo& requestInfo)
{
	return (requestInfo.requestID == static_cast<unsigned int>(RequestCode::GET_QUESTION_REQUEST)) ||
		(requestInfo.requestID == static_cast<unsigned int>(RequestCode::SUBMIT_ANSWER_REQUEST)) ||
		(requestInfo.requestID == static_cast<unsigned int>(RequestCode::GET_GAME_RESULTS_REQUEST)) ||
		(requestInfo.requestID == static_cast<unsigned int>(RequestCode::LEAVE_GAME_REQUEST));
}

RequestResult GameRequestHandler::handleRequest(const RequestInfo& requestInfo)
{
	return RequestResult();
}

void GameRequestHandler::handleDisconnection()
{
	// Disconnect the User from Login, Room and Game Managers
	//this->getFactorySafely()->getRoomManager().removeUserFromRoom() by GameId (Which will be RoomId) -> Unique!
	this->getFactorySafely()->getLoginManager().logOut(this->m_user.getUserName());
}

RequestResult GameRequestHandler::getQuestion(const RequestInfo& info)
{
	return RequestResult();
}

RequestResult GameRequestHandler::submitAnswer(const RequestInfo& info)
{
	return RequestResult();
}

RequestResult GameRequestHandler::getGameResults(const RequestInfo& info)
{
	return RequestResult();
}

RequestResult GameRequestHandler::leaveGame(const RequestInfo& info)
{
	return RequestResult();
}

std::shared_ptr<RequestHandlerFactory> GameRequestHandler::getFactorySafely()
{
	if (auto factory = this->m_handlerFactory.lock())
	{
		return factory;
	}

	throw ServerException("MenuRequestHandler: [ERROR]: RequestHandlerFactory is no longer available");
}

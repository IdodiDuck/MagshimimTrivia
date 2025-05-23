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
	try
	{
		switch (static_cast<RequestCode>(requestInfo.requestID))
		{
			case RequestCode::GET_QUESTION_REQUEST:
				return getQuestion(requestInfo);

			case RequestCode::SUBMIT_ANSWER_REQUEST:
				return submitAnswer(requestInfo);

			case RequestCode::GET_GAME_RESULTS_REQUEST:
				return getGameResults(requestInfo);

			case RequestCode::LEAVE_GAME_REQUEST:
				return leaveGame(requestInfo);

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
		// Stay in the same Handler, keep waiting for relevant request
		return requestResult;
	}

	catch (const ServerException& e)
	{
		ErrorResponse errorResponse = { };
		errorResponse.message = "Server Error: " + std::string(e.what());
		RequestResult requestResult;
		requestResult.response = JsonResponsePacketSerializer::serializeResponse(errorResponse);
		// Stay in the same Handler (For now)
		return requestResult;
	}
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

	throw ServerException("GameRequestHandler: [ERROR]: RequestHandlerFactory is no longer available");
}

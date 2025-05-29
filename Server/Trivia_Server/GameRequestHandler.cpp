#include "GameRequestHandler.h"

#include "RequestHandlerFactory.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"

#include "ManagerException.h"

#include <iostream>

GameRequestHandler::GameRequestHandler(std::weak_ptr<RequestHandlerFactory> handlerFactory, const LoggedUser& user, GameManager& gameManager, Game& game):
	m_handlerFactory(handlerFactory), m_user(user), m_gameManager(gameManager), m_game(game)
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
		requestResult.newHandler = this->getFactorySafely()->createGameRequestHandler(this->m_user, this->m_game);
		return requestResult;
	}

	catch (const ServerException& e)
	{
		ErrorResponse errorResponse = { };
		errorResponse.message = "Server Error: " + std::string(e.what());
		RequestResult requestResult;
		requestResult.response = JsonResponsePacketSerializer::serializeResponse(errorResponse);
		requestResult.newHandler = this->getFactorySafely()->createGameRequestHandler(this->m_user, this->m_game);
		return requestResult;
	}
}

void GameRequestHandler::handleDisconnection()
{
	try 
	{
		this->m_game.removePlayer(this->m_user.getUserName());
		this->getFactorySafely()->getRoomManager().removeUserFromRoom(this->m_game.getGameId(), this->m_user);

		if (this->m_game.isGameEmpty()) 
		{
			this->getFactorySafely()->getRoomManager().deleteRoom(this->m_game.getGameId());
			this->m_gameManager.deleteGame(this->m_game.getGameId());
		}
	}

	catch (const ManagerException& e) 
	{
		std::cout << e.what() << std::endl;
	}

	this->getFactorySafely()->getLoginManager().logOut(this->m_user.getUserName());
}

RequestResult GameRequestHandler::getQuestion(const RequestInfo& info)
{
	Question question = this->m_gameManager.getGameSafely(this->m_game.getGameId()).getQuestionForUser(this->m_user.getUserName());
	
	const std::vector<std::string>& answersVector = question.getPossibleAnswers();
	std::unordered_map<unsigned int, std::string> answersMap;

	for (unsigned int currentQuestionIndex = 0; currentQuestionIndex < answersVector.size(); currentQuestionIndex++)
	{
		answersMap[currentQuestionIndex] = answersVector[currentQuestionIndex];
	}

	GetQuestionResponse response = { SUCCESS, question.getQuestion(), answersMap};

	return
	{
		JsonResponsePacketSerializer::serializeResponse(response),
		this->getFactorySafely()->createGameRequestHandler(this->m_user, this->m_game)
	};

}

RequestResult GameRequestHandler::submitAnswer(const RequestInfo& info)
{
	SubmitAnswerRequest request = JsonRequestPacketDeserializer::deserializeSubmitAnswerRequest(info.buffer).value();

	SubmitAnswerResponse response = { };

	response.status = SUCCESS;
	Question currentQuestion = this->getFactorySafely()->getGameManager().getGameSafely(this->m_game.getGameId()).getQuestionForUser(this->m_user.getUserName());
	response.correctAnswerId = currentQuestion.getCorrectAnswerId();

	this->getFactorySafely()->getGameManager().getGameSafely(this->m_game.getGameId()).submitAnswer(this->m_user.getUserName(), currentQuestion.getPossibleAnswers().at(request.answerId));

	return
	{
		JsonResponsePacketSerializer::serializeResponse(response),
		this->getFactorySafely()->createGameRequestHandler(this->m_user, this->m_game)
	};
}

RequestResult GameRequestHandler::getGameResults(const RequestInfo& info)
{
	GetGameResultsResponse response = {};
	response.status = SUCCESS;

	std::vector<std::string> allPlayers = this->m_game.getAllPlayersUsernames();

	for (const std::string& username : allPlayers)
	{
		auto userGameData = this->m_game.getPlayerGameData(username);

		PlayerResults playerGameResult = {
			username,
			userGameData.correctAnswerCount,
			userGameData.wrongAnswerCount,
			userGameData.averageAnswerTime
		};

		response.results.push_back(playerGameResult);
	}

	return 
	{
		JsonResponsePacketSerializer::serializeResponse(response),
		this->getFactorySafely()->createMenuRequestHandler(this->m_user)
	};
}

RequestResult GameRequestHandler::leaveGame(const RequestInfo& info)
{
	try
	{
		this->m_game.removePlayer(this->m_user.getUserName());
		this->getFactorySafely()->getRoomManager().removeUserFromRoom(this->m_game.getGameId(), this->m_user.getUserName());
	}

	catch (const ManagerException& e)
	{	
		std::cout << e.what() << std::endl;
	}

	LeaveGameResponse leaveGameResponse;
	leaveGameResponse.status = SUCCESS;

	return
	{
		JsonResponsePacketSerializer::serializeResponse(leaveGameResponse),
		getFactorySafely()->createMenuRequestHandler(this->m_user.getUserName())
	};
}

std::shared_ptr<RequestHandlerFactory> GameRequestHandler::getFactorySafely()
{
	if (auto factory = this->m_handlerFactory.lock())
	{
		return factory;
	}

	throw ServerException("GameRequestHandler: [ERROR]: RequestHandlerFactory is no longer available");
}

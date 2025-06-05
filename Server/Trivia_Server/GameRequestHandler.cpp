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
		this->m_gameManager.submitGameStatsToDB(this->m_game.getPlayerGameData(this->m_user.getUserName()), this->m_user.getUserName());
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

	auto& game = this->getFactorySafely()->getGameManager().getGameSafely(this->m_game.getGameId());

	Question currentQuestion = game.getQuestionForUser(this->m_user.getUserName());
	auto possibleAnswers = currentQuestion.getPossibleAnswers();

	std::string answer = (request.answerId >= possibleAnswers.size() ? "NO_ANSWER" : possibleAnswers.at(request.answerId));
	response.correctAnswerId = currentQuestion.getCorrectAnswerId();

	this->m_game.submitAnswer(this->m_user.getUserName(), answer, request.answerTime);
	this->m_game.updateGame();

	return
	{
		JsonResponsePacketSerializer::serializeResponse(response),
		this->getFactorySafely()->createGameRequestHandler(this->m_user, this->m_game)
	};
}

RequestResult GameRequestHandler::getGameResults(const RequestInfo& info)
{
	GetGameResultsResponse response = {};

	this->m_game.updateGame();
	if (!this->m_game.isGameOver())
	{
		response.status = FAILURE;
		return
		{
			JsonResponsePacketSerializer::serializeResponse(response),
			this->getFactorySafely()->createGameRequestHandler(this->m_user, this->m_game)
		};
	}

	response.status = SUCCESS;

	auto playerResultsMap = this->m_game.getAllPlayerResults();

	for (const auto& [username, playerResult] : playerResultsMap)
	{
		response.results.push_back(playerResult);
	}

	return
	{
		JsonResponsePacketSerializer::serializeResponse(response),
		this->getFactorySafely()->createGameRequestHandler(this->m_user, this->m_game)
	};
}

RequestResult GameRequestHandler::leaveGame(const RequestInfo& info)
{
	try
	{
		this->m_gameManager.submitGameStatsToDB(this->m_game.getPlayerGameData(this->m_user.getUserName()), this->m_user.getUserName());
		this->m_game.removePlayer(this->m_user.getUserName());
		this->getFactorySafely()->getRoomManager().removeUserFromRoom(this->m_game.getGameId(), this->m_user.getUserName());

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

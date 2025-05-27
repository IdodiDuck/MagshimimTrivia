#pragma once

#include "Question.h"
#include "LoggedUser.h"

#include <vector>
#include <map>
#include <shared_mutex>

typedef struct GameData
{
    Question currentQuestion;
    unsigned int correctAnswerCount;
    unsigned int wrongAnswerCount;
    unsigned int averageAnswerTime;

} GameData;

enum class GameState
{
    WAITING,    
    IN_PROGRESS, 
    FINISHED     
};

class Game 
{

public:
	Game(int gameId, std::vector<Question> questions, std::map<std::string, GameData> users);

	Question getQuestionForUser(const std::string& user);
	void submitAnswer(const std::string& user, const std::string& answer);
	void removePlayer(const std::string& user);

private:
    std::vector<Question> m_questions;
    std::map<std::string, GameData> m_players;
    unsigned int m_gameId;
    unsigned int m_totalQuestions;
    GameState m_state;
    std::shared_mutex m_updateMutex;
    std::shared_mutex m_userMutex;

};
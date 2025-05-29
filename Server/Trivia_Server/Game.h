#pragma once

#include "Question.h"
#include "LoggedUser.h"

#include <vector>
#include <unordered_map>
#include <shared_mutex>
#include <chrono>

typedef struct GameData
{
    // Attributes - 
    Question currentQuestion;
    unsigned int correctAnswerCount;
    unsigned int wrongAnswerCount;
    unsigned int averageAnswerTime;

    // Default C'tor - 
    GameData() = default;

} GameData;

enum class GameState
{
    WAITING_TO_START,
    STARTED,
    WAITING_FOR_NEXT_QUESTION,
    WAITING_FOR_ANSWER,
    FINISHED
};

class Game 
{

public:
    // C'tor - 
	Game(unsigned int gameId, std::vector<Question> questions, std::unordered_map<std::string, GameData> users);

	Question getQuestionForUser(const std::string& user);
	void submitAnswer(const std::string& user, const std::string& answer);
	void removePlayer(const std::string& user);

    // Getters - 
    unsigned int getGameId() const;
    bool isGameEmpty() const;
    GameData getPlayerGameData(const std::string& username) const;
    std::vector<std::string> getAllPlayersUsernames() const;

private:
    // Attributes - 
    std::vector<Question> m_questions;
    std::unordered_map<std::string, GameData> m_players;
    unsigned int m_gameId;
    unsigned int m_totalQuestions;
    GameState m_state;
    mutable std::shared_mutex m_updateMutex;
    mutable std::shared_mutex m_userMutex;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> m_answerTimes;

};
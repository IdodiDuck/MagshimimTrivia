#pragma once

#include "Question.h"
#include "LoggedUser.h"
#include "JsonResponsePacketSerializer.h"

#include <vector>
#include <unordered_map>
#include <shared_mutex>

typedef struct GameData
{
    Question currentQuestion;
    unsigned int correctAnswerCount = 0;
    unsigned int wrongAnswerCount = 0;
    double averageAnswerTime = 0.0;
    bool hasLeft = false;
    bool hasAnsweredCurrentQuestion = false;

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

enum class AnswerResult
{
    CORRECT,
    WRONG,
    NO_ANSWER
};

class Game
{
public:
    // C'tor & D'tor - 
    Game(unsigned int gameId, std::vector<Question> questions, std::unordered_map<std::string, GameData> users, const unsigned int timePerQuestion);
    ~Game();

    Question getQuestionForUser(const std::string& user);
    void submitAnswer(const std::string& user, const std::string& answer, const double answerTime);
    void removePlayer(const std::string& user);

    unsigned int getGameId() const;
    bool isGameEmpty() const;
    bool isGameOver() const;
    GameData getPlayerGameData(const std::string& username) const;
    std::unordered_map<std::string, PlayerResults> getAllPlayerResults() const;

private:
    // Attributes 
    std::vector<Question> m_questions;
    std::unordered_map<std::string, GameData> m_players;
    unsigned int m_gameId;
    unsigned int m_totalQuestions;
    GameState m_state;

    mutable std::shared_mutex m_updateMutex;
    mutable std::shared_mutex m_userMutex;

    std::unordered_map<std::string, double> m_answerTimes;

    // Support Methods
    bool isUserActive(const std::string& user) const;
    bool didUserAnswer(const std::string& user) const;
    unsigned int getCurrentQuestionIndex(const GameData& data) const;
    void updateUserStatistics(GameData& data, const std::string& answer, const double time);

    void updateGame();

    friend class GameRequestHandler;
};
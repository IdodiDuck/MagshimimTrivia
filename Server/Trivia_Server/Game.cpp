#include "Game.h"

Game::Game(int gameId, std::vector<Question> questions, std::map<std::string, GameData> users)
    : m_gameId(gameId), m_questions(questions), m_players(users), m_totalQuestions((unsigned int)questions.size()), m_state(GameState::WAITING)
{
    if (!users.empty()) 
    {
        m_state = GameState::IN_PROGRESS;
    }
}

Question Game::getQuestionForUser(const std::string& user)
{
    std::shared_lock lock(m_userMutex);

    if (m_players.find(user) == m_players.end())
    {
        throw std::runtime_error("User not found in game");
    }

    GameData& data = m_players[user];
    unsigned int currentIndex = data.correctAnswerCount + data.wrongAnswerCount;

    if (currentIndex >= m_questions.size()) 
    {
        throw std::runtime_error("No more questions available for this user");
    }

    data.currentQuestion = m_questions[currentIndex];
    return data.currentQuestion;
}

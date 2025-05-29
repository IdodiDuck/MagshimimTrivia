#include "Game.h"

#include "ManagerException.h"

Game::Game(unsigned int gameId, std::vector<Question> questions, std::unordered_map<std::string, GameData> users)
    : m_gameId(gameId), m_questions(questions), m_players(users), m_totalQuestions(static_cast<unsigned int>(questions.size())), m_state(GameState::WAITING)
{
    if (!users.empty()) 
    {
        m_state = GameState::IN_PROGRESS;
    }
}

Question Game::getQuestionForUser(const std::string& user)
{
    std::shared_lock lock(m_userMutex);

    if (m_players.find(user) == m_players.cend())
    {
        throw ManagerException("User not found in game");
    }

    GameData& data = m_players.at(user);
    unsigned int currentIndex = data.correctAnswerCount + data.wrongAnswerCount;

    if (currentIndex >= m_questions.size()) 
    {
        throw ManagerException("No more questions available for this user");
    }

    data.currentQuestion = m_questions[currentIndex];
    return data.currentQuestion;
}

void Game::submitAnswer(const std::string& user, const std::string& answer)
{
    std::unique_lock lock(m_updateMutex);

    if (m_players.find(user) == m_players.end()) 
    {
        throw ManagerException("User not found in game");
    }

    GameData& data = m_players.at(user);
    unsigned int currentIndex = data.correctAnswerCount + data.wrongAnswerCount;

    if (currentIndex >= m_questions.size()) 
    {
        throw std::runtime_error("User has already answered all questions");
    }

    auto now = std::chrono::steady_clock::now();
    unsigned int answerTime = 0;
    if (m_answerTimes.find(user) != m_answerTimes.end())
    {
        answerTime = static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::seconds>(now - m_answerTimes[user]).count());
        m_answerTimes.erase(user);
    }

    const std::string& correctAnswer = data.currentQuestion.getCorrectAnswer();
    if (answer == correctAnswer) 
    {
        data.correctAnswerCount++;
    }
    else 
    {
        data.wrongAnswerCount++;
    }

    unsigned int totalAnswered = data.correctAnswerCount + data.wrongAnswerCount;
    data.averageAnswerTime = ((data.averageAnswerTime * (totalAnswered - 1)) + answerTime) / totalAnswered;

    bool allFinished = true;

    for (const auto& [username, gameData] : m_players) 
    {
        if ((gameData.correctAnswerCount + gameData.wrongAnswerCount) < m_totalQuestions)
        {
            allFinished = false;
            break;
        }
    }

    if (allFinished) 
    {
        m_state = GameState::FINISHED;
    }
}

void Game::removePlayer(const std::string& user)
{
    std::unique_lock lock(m_userMutex);
    m_players.erase(user);
}
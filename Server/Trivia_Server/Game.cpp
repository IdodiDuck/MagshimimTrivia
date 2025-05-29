#include "Game.h"

#include "ManagerException.h"

Game::Game(unsigned int gameId, std::vector<Question> questions, std::unordered_map<std::string, GameData> users):
    m_gameId(gameId), m_questions(questions), m_players(users), m_totalQuestions(static_cast<unsigned int>(questions.size())), m_state(GameState::WAITING_TO_START)
{
    if (!users.empty()) 
    {
        this->m_state = GameState::STARTED;
    }
}

Question Game::getQuestionForUser(const std::string& user)
{
    std::shared_lock lock(m_updateMutex);

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
    m_answerTimes[user] = std::chrono::steady_clock::now();

    return data.currentQuestion;
}

void Game::submitAnswer(const std::string& user, const std::string& answer)
{
    std::unique_lock lock(m_updateMutex);

    if (m_players.find(user) == m_players.cend()) 
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
    if (m_answerTimes.find(user) != m_answerTimes.cend())
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

unsigned int Game::getGameId() const
{
    return this->m_gameId;
}

bool Game::isGameEmpty() const
{
    const int EMPTY = 0;

    return (this->m_players.size() == EMPTY);
}

GameData Game::getPlayerGameData(const std::string& username) const
{
    std::shared_lock readLock(m_userMutex);

    if (this->m_players.find(username) == this->m_players.cend()) 
    {
        throw ManagerException("User not found in game");
    }

    return this->m_players.at(username);
}

std::vector<std::string> Game::getAllPlayersUsernames() const
{
    std::shared_lock lock(m_userMutex);
    std::vector<std::string> usernames;

    for (const auto& [name, data] : m_players)
    {
        usernames.push_back(name);
    }

    return usernames;
}

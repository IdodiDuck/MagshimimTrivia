#include "Game.h"

#include "ManagerException.h"

Game::Game(const unsigned int gameId, std::vector<Question> questions, std::unordered_map<std::string, GameData> users, const unsigned int timePerQuestion) :
    m_gameId(gameId), m_questions(questions), m_players(users), m_totalQuestions(static_cast<unsigned int>(questions.size())), m_state(GameState::WAITING_TO_START)
{
    if (!users.empty())
    {
        this->m_state = GameState::STARTED;
    }
}

Game::~Game()
{
    this->m_questions.clear();
    this->m_players.clear();
    this->m_answerTimes.clear();
}

Question Game::getQuestionForUser(const std::string& user)
{
    std::shared_lock lock(m_updateMutex);

    if (!isUserActive(user))
    {
        throw ManagerException("User not found in game");
    }

    GameData& data = m_players.at(user);
    unsigned int index = getCurrentQuestionIndex(data);

    if (index >= m_questions.size())
    {
        throw ManagerException("No more questions for this user");
    }

    Question& question = m_questions[index];
    data.currentQuestion = question;
    m_answerTimes[user] = 0.00; // Resets the answerTime when receiving new valid question request

    return question;
}

void Game::submitAnswer(const std::string& user, const std::string& answer, const double answerTime)
{
    std::unique_lock lock(m_updateMutex);

    if (!isUserActive(user))
    {
        throw ManagerException("User not found in game");
    }

    GameData& data = m_players.at(user);
    unsigned int index = getCurrentQuestionIndex(data);

    if (index >= m_questions.size())
    {
        throw std::runtime_error("User has already answered all questions");
    }

    m_answerTimes.find(user)->second = answerTime;
    updateUserStatistics(data, answer, answerTime);

    this->m_state = GameState::WAITING_FOR_NEXT_QUESTION;

    lock.unlock();
    updateGame();
}

void Game::removePlayer(const std::string& user)
{
    std::unique_lock lock(m_userMutex);

    auto removedUserIt = m_players.find(user);
    if (removedUserIt != m_players.cend())
    {
        removedUserIt->second.hasLeft = true;
    }
}

unsigned int Game::getGameId() const
{
    return this->m_gameId;
}

bool Game::isGameEmpty() const
{
    std::shared_lock readLock(m_userMutex);

    for (const auto& [username, data] : m_players)
    {
        if (!data.hasLeft)
        {
            return false;
        }
    }

    return true;
}

GameData Game::getPlayerGameData(const std::string& username) const
{
    std::shared_lock readLock(m_userMutex);

    if (!isUserActive(username))
    {
        throw ManagerException("User not found in game");
    }

    return this->m_players.at(username);
}

std::unordered_map<std::string, PlayerResults> Game::getAllPlayerResults() const
{
    std::unordered_map<std::string, PlayerResults> results;

    for (const auto& [username, data] : this->m_players)
    {
        results[username] = PlayerResults{
            username,
            data.correctAnswerCount,
            data.wrongAnswerCount,
            data.averageAnswerTime
        };
    }

    return results;
}

bool Game::isUserActive(const std::string& user) const
{
    auto userIt = m_players.find(user);
    return (userIt != m_players.cend()) && (!userIt->second.hasLeft);
}

bool Game::didUserAnswer(const std::string& user) const
{
    std::shared_lock readLock(m_userMutex);

    if (!isUserActive(user))
    {
        return false;
    }

    return (m_answerTimes.find(user) != m_answerTimes.cend());
}

void Game::updateGame()
{
    switch (this->m_state)
    {
        case GameState::WAITING_FOR_ANSWER:
        {
            for (auto& playerPair : m_players)
            {
                const std::string& username = playerPair.first;

                if (!didUserAnswer(username))
                {
                    submitAnswer(username, "NO_ANSWER", m_answerTimes.find(username)->second);
                }
            }

            m_state = GameState::WAITING_FOR_NEXT_QUESTION;
            break;
        }

        case GameState::WAITING_FOR_NEXT_QUESTION:
        {
            bool allPlayersDone = didAllPlayersFinish();

            this->m_state = allPlayersDone ? GameState::FINISHED : GameState::WAITING_FOR_ANSWER;
            break;
        }

        default:
            break;
    }
}

bool Game::isGameOver() const
{
    return (this->m_state == GameState::FINISHED);
}

unsigned int Game::getCurrentQuestionIndex(const GameData& data) const
{
    return data.correctAnswerCount + data.wrongAnswerCount;
}

void Game::updateUserStatistics(GameData& data, const std::string& answer, const double time)
{
    const std::string& correctAnswer = data.currentQuestion.getCorrectAnswer();
    const int ONE_ANSWER = 1;

    bool isCorrect = (answer == correctAnswer);

    isCorrect ? data.correctAnswerCount++ : data.wrongAnswerCount++;

    double totalAnswered = static_cast<double>(data.correctAnswerCount + data.wrongAnswerCount);

    if (totalAnswered == ONE_ANSWER)
    {
        data.averageAnswerTime = time;
        return;
    }

    data.averageAnswerTime = ((data.averageAnswerTime * (totalAnswered - 1)) + time) / totalAnswered;
}

bool Game::didAllPlayersFinish()
{
    for (const auto& [user, data] : this->m_players)
    {
        unsigned int currentQuestion = data.correctAnswerCount + data.wrongAnswerCount;
        if (currentQuestion < m_totalQuestions)
        {
            return false;
        }
    }

    return true;
}

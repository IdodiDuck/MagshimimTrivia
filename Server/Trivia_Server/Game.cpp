#include "Game.h"

#include "ManagerException.h"

Game::Game(const unsigned int gameId, std::vector<Question> questions, std::unordered_map<std::string, GameData> users, const unsigned int timePerQuestion) :
    m_gameId(gameId), m_questions(questions), m_players(users), m_totalQuestions(static_cast<unsigned int>(questions.size())), m_state(GameState::WAITING_TO_START), m_timerDuration(std::chrono::seconds(timePerQuestion))
{
    if (!users.empty())
    {
        this->m_state = GameState::STARTED;
        this->m_timerStart = std::chrono::steady_clock::now();
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
        throw std::out_of_range("No more questions for this user");
    }

    Question& question = m_questions[index];
    data.currentQuestion = question;
    m_answerTimes[user] = std::chrono::steady_clock::now();

    return question;
}

void Game::submitAnswer(const std::string& user, const std::string& answer)
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

    unsigned int answerTime = calculateAnswerTime(user);

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

bool Game::isTimerExpired(const std::chrono::seconds duration) const
{
    auto now = std::chrono::steady_clock::now();
    return (elapsedTime() >= duration);
}

std::chrono::seconds Game::timeLeft() const
{
    auto now = std::chrono::steady_clock::now();
    return std::max(std::chrono::seconds(0), this->timeDurationWait() - elapsedTime());
}

std::chrono::seconds Game::elapsedTime() const
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now - m_timerStart);
}

std::chrono::seconds Game::timeDurationWait() const
{
    return (this->m_state == GameState::WAITING_FOR_ANSWER ? this->m_timerDuration : this->m_waitingForQuestionDuration);
}

void Game::updateGame()
{
    if (!this->isTimerExpired(timeDurationWait()))
    {
        return;
    }

    switch (this->m_state)
    {
    case GameState::WAITING_FOR_ANSWER:
    {
        for (auto& playerPair : m_players)
        {
            const std::string& username = playerPair.first;

            if (!didUserAnswer(username))
            {
                submitAnswer(username, "NO_ANSWER");
            }
        }

        m_state = GameState::WAITING_FOR_NEXT_QUESTION;
        m_timerStart = std::chrono::steady_clock::now();
        break;
    }

    case GameState::WAITING_FOR_NEXT_QUESTION:
    {
        bool allPlayersDone = true;

        for (const auto& [user, data] : m_players)
        {
            unsigned int currentQuestion = data.correctAnswerCount + data.wrongAnswerCount;
            if (currentQuestion < m_totalQuestions)
            {
                allPlayersDone = false;
                break;
            }
        }

        this->m_state = allPlayersDone ? GameState::FINISHED : GameState::WAITING_FOR_ANSWER;
        m_timerStart = std::chrono::steady_clock::now();
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

double Game::calculateAnswerTime(const std::string& user)
{
    auto now = std::chrono::steady_clock::now();

    if (auto it = m_answerTimes.find(user); it != m_answerTimes.end())
    {
        double time = static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count());
        m_answerTimes.erase(it);
        return time;
    }

    return 0.0;
}

void Game::updateUserStatistics(GameData& data, const std::string& answer, const double time)
{
    const std::string& correctAnswer = data.currentQuestion.getCorrectAnswer();
    bool isCorrect = (answer == correctAnswer);

    isCorrect ? data.correctAnswerCount++ : data.wrongAnswerCount++;

    double totalAnswered = static_cast<double>(data.correctAnswerCount + data.wrongAnswerCount);

    if (totalAnswered == 1)
    {
        data.averageAnswerTime = time;
        return;
    }

    data.averageAnswerTime = ((data.averageAnswerTime * (totalAnswered - 1)) + time) / totalAnswered;
}

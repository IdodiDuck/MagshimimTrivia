#include "GameManager.h"

#include "ManagerException.h"

GameManager::GameManager(std::weak_ptr<IDatabase> dataBase)
    : m_Database(dataBase)
{
}

Game& GameManager::createGame(const Room& room)
{
    std::unique_lock lock(m_mutex);

    auto dataBase = m_Database.lock();

    if (!dataBase)
    {
        throw ServerException("[ERROR] Failed to open database in constructor.");
    }

    const auto& roomData = room.getRoomData();
    auto gameId = roomData.id;
    auto questionCount = roomData.numOfQuestionsInGame;

    auto questionsFromDataBase= dataBase->getQuestions(questionCount);

    if (questionsFromDataBase.empty())
    {
        throw ManagerException("No questions available for game!");
    }

    std::unordered_map<std::string, GameData> players;
    std::vector<Question> questions = std::vector<Question>(questionsFromDataBase.cbegin(), questionsFromDataBase.cend());

    const unsigned int NONE = 0;

    for (const auto& user : room.getAllUsers()) 
    {
        players[user] = GameData { questions.front(), NONE, NONE, NONE };
    }

    this->m_games.try_emplace(gameId, gameId, std::move(questions), std::move(players));

    return this->getGameSafely(gameId);
}

void GameManager::deleteGame(const unsigned int gameId)
{
    std::unique_lock lock(m_mutex);

    if (!m_games.erase(gameId))
    {
        throw ManagerException("Game not found, cannot delete it!");
    }
}

Game& GameManager::getGameSafely(const unsigned int gameId)
{
    auto dataBase = m_Database.lock();

    if (!dataBase)
    {
        throw ServerException("[ERROR] Failed to open database in constructor.");
    }

    std::shared_lock readLock(m_mutex);

    if (this->m_games.find(gameId) == this->m_games.cend()) 
    {
        throw ManagerException("Error: Game not found.");
    }

    return this->m_games.at(gameId);
}

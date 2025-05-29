#include "GameManager.h"

#include <stdexcept>

GameManager::GameManager(std::weak_ptr<IDatabase> db)
    : m_IDatabase(db)
{
}

std::shared_ptr<Game> GameManager::createGame(const Room& room)
{
    std::unique_lock lock(m_mutex);

    auto db = m_IDatabase.lock();
    if (!db) 
    {
        throw std::runtime_error("Database connection lost");
    }

    const auto& roomData = room.getRoomData();
    auto gameId = roomData.id;
    auto questionCount = roomData.numOfQuestionsInGame;

    auto questions = db->getQuestions(questionCount);
    if (questions.empty())
    {
        throw std::runtime_error("No questions available for game");
    }

    std::unordered_map<std::string, GameData> players;
    for (const auto& user : room.getAllUsers()) 
    {
        players[user] = GameData{ questions.front(), 0, 0, 0 };  
    }

    auto game = std::make_shared<Game>(gameId, questions, players);
    m_games[gameId] = game;

    return std::make_shared<Game>(m_games[gameId]);
}

void GameManager::deleteGame(const unsigned int gameId)
{
    std::unique_lock lock(m_mutex);
    if (m_games.erase(gameId) == 0)
    {
        throw std::runtime_error("Game not found, cannot delete");
    }
}

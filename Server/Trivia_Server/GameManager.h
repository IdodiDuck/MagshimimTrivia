#pragma once

#include "Game.h"
#include "IDatabase.h"
#include "Room.h"

#include <memory>
#include <unordered_map>
#include <shared_mutex>

class GameManager
{

public:
	// C'tor - 
	GameManager(std::weak_ptr<IDatabase> dataBase);

	// Game Management Methods - 
	Game& createGame(const Room& room);
	void deleteGame(const unsigned int gameId);
	Game& getGameSafely(const unsigned int gameId);
	

private:
	// Attributes - 
	std::weak_ptr<IDatabase> m_Database;
	std::unordered_map<int, Game> m_games;
	std::shared_mutex m_mutex;

};


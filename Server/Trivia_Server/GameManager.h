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
	GameManager(std::weak_ptr<IDatabase> db);
	std::shared_ptr<Game> createGame(const Room& room);
	void deleteGame(const unsigned int gameId);

private:
	std::weak_ptr<IDatabase> m_IDatabase;
	std::unordered_map<int, std::shared_ptr<Game>> m_games;
	std::shared_mutex m_mutex;

};


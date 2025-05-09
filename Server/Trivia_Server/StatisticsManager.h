#pragma once

#include "IDatabase.h"

#include <string>
#include <vector>
#include <memory>

class StatisticsManager
{


public:
	StatisticsManager(std::weak_ptr<IDatabase> dataBase);
	std::vector<std::string> getHighScore();
	std::vector<std::string> getUserStatistics(const std::string& username);

private:

	std::weak_ptr<IDatabase> m_database;

};


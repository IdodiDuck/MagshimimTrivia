#pragma once

#include "IDatabase.h"
#include <string>
#include <vector>


class StatisticsManager
{


public:
	StatisticsManager(std::weak_ptr<IDatabase> dataBase);
	~StatisticsManager();
	std::vector<std::string> getHighScore();
	std::vector<std::string> getUserStatistics(const std::string& username);

private:

	std::weak_ptr<IDatabase> m_database;

};


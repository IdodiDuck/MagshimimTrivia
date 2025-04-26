#pragma once

#include "IDatabase.h"
#include <string>
#include <vector>


class StatisticsManager
{

public:

	std::vector<std::string> getHighScore();
	std::vector<std::string> getUserStatistics(const std::string& username);

private:

	IDatabase* m_database;

};


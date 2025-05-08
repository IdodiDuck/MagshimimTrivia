#include "StatisticsManager.h"

#include <algorithm>
#include <sstream>
#include <iostream>

StatisticsManager::StatisticsManager(std::weak_ptr<IDatabase> dataBase): m_database(dataBase)
{

}

std::vector<std::string> StatisticsManager::getHighScore()
{
    auto db = m_database.lock();
    return db->getHighScores();
}

std::vector<std::string> StatisticsManager::getUserStatistics(const std::string& username)
{
    std::vector<std::string> stats;
    auto db = m_database.lock();

    const int totalGames = db->getNumOfPlayerGames(username);
    const int correctAnswers = db->getNumOfCorrectAnswers(username);
    const int totalAnswers = db->getNumOfTotalAnswers(username);
    const float avgAnswerTime = db->getPlayerAverageAnswerTime(username);
    const int playerScore = db->getPlayerScore(username);
    const float accuracy = (totalAnswers > 0) ? (static_cast<float>(correctAnswers) / totalAnswers) * 100 : 0.0f;

    stats.push_back("Player: " + username);
    stats.push_back("Total Score: " + std::to_string(playerScore));
    stats.push_back("Games Played: " + std::to_string(totalGames));
    stats.push_back("Correct Answers: " + std::to_string(correctAnswers) + "/" + std::to_string(totalAnswers));

    std::stringstream acc;
    acc << accuracy;
    stats.push_back("Accuracy: " + acc.str() + "%");
    std::stringstream time;
    time << avgAnswerTime;
    stats.push_back("Average Answer Time: " + time.str() + "s");

    return stats;
}

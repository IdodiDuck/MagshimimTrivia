#include "StatisticsManager.h"

#include <algorithm>
#include <sstream>

std::vector<std::string> StatisticsManager::getHighScore()
{
    std::vector<std::string> scores = m_database->getHighScores();

    std::sort(scores.begin(), scores.end(), [](const std::string& a, const std::string& b) 
        {
            return std::stoi(a) > std::stoi(b);
        });

    if (scores.size() > 5) 
    {
        scores.resize(5);
    }

    return scores;
}

std::vector<std::string> StatisticsManager::getUserStatistics(const std::string& username)
{
    std::vector<std::string> stats;

    const int totalGames = m_database->getNumOfPlayerGames(username);
    const int correctAnswers = m_database->getNumOfCorrectAnswers(username);
    const int totalAnswers = m_database->getNumOfTotalAnswers(username);
    const float avgAnswerTime = m_database->getPlayerAverageAnswerTime(username);
    const int playerScore = m_database->getPlayerScore(username);
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

#pragma once

#include <string>
#include <list>
#include <vector>

#include "Question.h"
#include "Game.h"

class IDatabase
{
public:
    virtual ~IDatabase() = default;

    // Connection
    virtual bool open() = 0;
    virtual bool close() = 0;

    // User handling - 
    virtual bool doesUserExist(const std::string& user) = 0;
    virtual bool doesUserExistInStatistics(const std::string& user) = 0;
    virtual int doesPasswordMatch(const std::string& user, const std::string& password) = 0;
    virtual int addNewUser(const std::string& user, const std::string& password, const std::string& email) = 0;

    // Questions - 
    virtual std::list<Question> getQuestions(const int questionsAmount) = 0;

    // Player statistics - 
    virtual float getPlayerAverageAnswerTime(const std::string& username) = 0;
    virtual int getNumOfCorrectAnswers(const std::string& username) = 0;
    virtual int getNumOfTotalAnswers(const std::string& username) = 0;
    virtual int getNumOfPlayerGames(const std::string& username) = 0;
    virtual int getPlayerScore(const std::string& username) = 0;
    virtual int submitGameStatistics(const std::string& username, const GameData& data) = 0;

    // Global stats - 
    virtual std::vector<std::string> getHighScores() = 0;
};

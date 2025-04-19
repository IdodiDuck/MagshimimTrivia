#pragma once

#include "IDatabase.h"

#include "sqlite3.h"

#include <nlohmann/json.hpp>

enum class DatabaseResult
{
	DATABASE_ERROR = -1,
	DATABASE_SUCCESS,
	USER_EXISTS,
	USER_NOT_FOUND,
	PASSWORD_MATCH,
	PASSWORD_DISMATCH
};


class SqliteDataBase : public IDatabase
{

public:

	// C'tor & D'tor - 
	SqliteDataBase();
	virtual ~SqliteDataBase();

	// Connection -
	bool open() override;
	bool close() override;

	// User handling - 
	int doesUserExist(const std::string& user) override;
	int doesPasswordMatch(const std::string& user, const std::string& password) override;
	int addNewUser(const std::string& user, const std::string& password, const std::string& email) override;

	// Questions - 
	std::list<Question> getQuestions(const int questionsAmount) override;

	// Player statistics - 
	float getPlayerAverageAnswerTime(const std::string& username) override;
	int getNumOfCorrectAnswers(const std::string& username) override;
	int getNumOfTotalAnswers(const std::string& username) override;
	int getNumOfPlayerGames(const std::string& username) override;
	int getPlayerScore(const std::string& username) override;

	// Global Stats - 
	std::vector<std::string> getHighScores() override;

private:
	sqlite3* _dataBase;
	std::string _dataBaseName;

	// Support Methods - 
	int executeQuery(const std::string& sql);
	void initializeTriviaDB();
	bool isDataBaseOpen();

	// Adding 10 Questions to Database (V2.0.0)
	void addQuestionsFromOpenTDB();
	void insertQuestionsIntoDataBase(const nlohmann::json& jsonResponse);
	
	std::string decodeHtmlCharacters(const std::string& input);
	void prepareSQLQuery(std::string& input);

	static size_t writeGETResponse(void* ptr, size_t size, size_t nmemb, void* data);
};

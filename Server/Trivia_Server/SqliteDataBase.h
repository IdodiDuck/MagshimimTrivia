#pragma once

#include "IDatabase.h"

#include "sqlite3.h"

enum class DatabaseResult
{
	DATABASE_ERROR = -1,
	DATABASE_SUCCESS,
	USER_EXISTS,
	USER_NOT_FOUND,
	PASSWORD_MATCH,
	PASSWORD_DISMATCH
};


class SqliteDataBase : IDatabase
{

public:

	SqliteDataBase();
	virtual ~SqliteDataBase();

	bool open() override;
	bool close() override;

	int doesUserExist(const std::string& user) override;
	int doesPasswordMatch(const std::string& user, const std::string& password) override;
	int addNewUser(const std::string& user, const std::string& password, const std::string& email) override;

private:
	sqlite3* _dataBase;
	std::string _dataBaseName;

	// Support Methods - 
	int executeQuery(const std::string& sql);
	void initializeTriviaDB();
	bool isDataBaseOpen();
};

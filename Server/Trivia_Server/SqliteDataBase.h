#pragma once

#include "IDatabase.h"

#include "sqlite3.h"
#include <string>
#include <io.h>
#include <iostream>

enum dbResult
{
	DATABASE_ERROR = -1,
	USER_EXISTS,
	USER_NOT_FOUND,
	PASSWORD_MATCH,
	PASSWORD_DISMATCH
};

const std::string TRIVIA_DB = "trivia.db";

sqlite3* db;

int executeQuery(const std::string& sql);
void initializeTriviaDB();


class SqliteDataBase : IDatabase
{
public:

	bool open() override;
	bool close() override;

	int doesUserExist(const std::string& user) override;
	int doesPasswordMatch(const std::string& user, const std::string& password) override;
	int addNewUser(const std::string& user, const std::string& password, const std::string& email) override;

};

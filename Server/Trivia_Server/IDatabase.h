#pragma once

#include <string>

class IDatabase
{
public: 

	virtual ~IDatabase() = default;

	virtual bool open() = 0;
	virtual bool close() = 0;

	virtual int doesUserExist(const std::string& user) = 0;
	virtual int doesPasswordMatch(const std::string& user, const std::string& passsword) = 0;
	virtual int addNewUser(const std::string& user, const std::string& password, const std::string& email) = 0;

};


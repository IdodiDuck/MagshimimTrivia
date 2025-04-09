#pragma once

#include <string>

class LoggedUser
{

public:

	// C'tors - 
	LoggedUser(const std::string& username);

	// Getters - 
	std::string getUserName() const;

	// Operators - 
	bool operator==(const LoggedUser& otherUser) const;


private:
	// Attributes - 
	std::string m_username;
};


#pragma once

#include "ServerException.h"

class ManagerException : public ServerException
{
public:
	// C'tor
	ManagerException(const std::string& errorMessage);

	// Virtuals - 
	const char* what() const noexcept;

};


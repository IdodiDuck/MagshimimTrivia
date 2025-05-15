#pragma once

#include "ServerException.h"

class ClientConnectionException : public ServerException
{
public:
	// C'tor
	ClientConnectionException(const std::string& errorMessage);

	// Virtuals - 
	const char* what() const noexcept;
};


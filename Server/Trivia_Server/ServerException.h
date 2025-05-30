#pragma once

#include <exception>
#include <string>

class ServerException : public std::exception
{

public:
	// C'tor & D'tor - 
	ServerException(const std::string& errorMessage);
	virtual ~ServerException() noexcept = default;

	// Virtuals - 
	const char* what() const noexcept override;

protected:
	std::string m_errorMessage;
};

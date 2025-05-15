#include "ServerException.h"

ServerException::ServerException(const std::string& errorMessage): m_errorMessage(errorMessage)
{

}

const char* ServerException::what() const noexcept
{
	return m_errorMessage.c_str();
}

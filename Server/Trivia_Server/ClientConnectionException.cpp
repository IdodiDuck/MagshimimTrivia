#include "ClientConnectionException.h"

ClientConnectionException::ClientConnectionException(const std::string& errorMessage): ServerException(errorMessage)
{

}

const char* ClientConnectionException::what() const noexcept
{
    return m_errorMessage.c_str();
}

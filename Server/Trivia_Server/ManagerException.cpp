#include "ManagerException.h"

ManagerException::ManagerException(const std::string& errorMessage): ServerException(errorMessage)
{

}

const char* ManagerException::what() const noexcept
{
    return this->m_errorMessage.c_str();
}

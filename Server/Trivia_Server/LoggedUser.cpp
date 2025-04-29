#include "LoggedUser.h"

LoggedUser::LoggedUser(const std::string& username): m_username(username)
{

}

std::string LoggedUser::getUserName() const
{
	return this->m_username;
}

bool LoggedUser::operator==(const LoggedUser& otherUser) const
{
	return (this->m_username == otherUser.getUserName());
}

bool LoggedUser::operator<(const LoggedUser& other) const
{
	return (m_username < other.m_username);
}

#include "LoginManager.h"

#include "SqliteDataBase.h"

#include <algorithm>

LoginManager::LoginManager(std::weak_ptr<IDatabase> dataBase): m_dataBase(dataBase)
{

}

LoginManager::~LoginManager()
{
	this->m_loggedUsers.clear();
}

SignUpStatus LoginManager::signUp(const std::string& username, const std::string& password, const std::string& email)
{
    if (auto dataBase = m_dataBase.lock()) 
    {
        int userExists = dataBase->doesUserExist(username);

        if (userExists == static_cast<int>(DatabaseResult::USER_EXISTS))
        {
            return SignUpStatus::USER_ALREADY_EXISTS;
        }

        int dataBaseResult = dataBase->addNewUser(username, password, email);

        return (dataBaseResult == static_cast<int>(DatabaseResult::DATABASE_SUCCESS) ? SignUpStatus::SUCCESS : SignUpStatus::ERROR);
    }

    return SignUpStatus::ERROR;
}

LoginStatus LoginManager::login(const std::string& username, const std::string& password)
{
    if (isUserAlreadyLogged(username))
    {
        return LoginStatus::USER_ALREADY_LOGGED_IN;
    }

    if (auto dataBase = m_dataBase.lock())
    {
        int userExists = dataBase->doesUserExist(username);

        if (userExists == static_cast<int>(DatabaseResult::USER_NOT_FOUND))
        {
            return LoginStatus::USER_NOT_EXISTS;
        }

        int passwordState = dataBase->doesPasswordMatch(username, password);

        if (passwordState == static_cast<int>(DatabaseResult::PASSWORD_MATCH))
        {
            std::lock_guard<std::mutex> lock(this->m_loggedUsersMutex);
            this->m_loggedUsers.push_back(LoggedUser(username));

            return LoginStatus::SUCCESS;
        }

        return LoginStatus::DISMATCHING_PASSWORD;
    }

    return LoginStatus::ERROR;
}

void LoginManager::logOut(const std::string& username)
{
    std::lock_guard<std::mutex> lock(this->m_loggedUsersMutex);

    auto loggedOutUserIt = std::find_if(this->m_loggedUsers.begin(), this->m_loggedUsers.end(), [&](const LoggedUser& user) 
    {
        return user.getUserName() == username;
    });

    // If we found the user we remove it from the logged users
    if (loggedOutUserIt != m_loggedUsers.cend())
    {
        this->m_loggedUsers.erase(loggedOutUserIt);
    }
}

bool LoginManager::isUserAlreadyLogged(const std::string& username)
{
    std::lock_guard<std::mutex> lock(this->m_loggedUsersMutex);

    auto userAlreadyLoggedIn = std::find_if(this->m_loggedUsers.begin(), this->m_loggedUsers.end(), [&](const LoggedUser& user)
    {
        return user.getUserName() == username;
    });

    return (userAlreadyLoggedIn != m_loggedUsers.cend());
}

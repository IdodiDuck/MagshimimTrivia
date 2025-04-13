#include "LoginManager.h"

#include "SqliteDataBase.h"

#include <algorithm>
#include <iostream>

LoginManager::LoginManager(std::weak_ptr<IDatabase> dataBase): m_dataBase(dataBase)
{

}

LoginManager::~LoginManager()
{
	this->m_loggedUsers.clear();
}

SignUpStatus LoginManager::signUp(const std::string& username, const std::string& password, const std::string& email)
{
    std::cout << "Attempting to signup user with username: " << username << std::endl;
    if (auto dataBase = m_dataBase.lock()) 
    {
        int userExists = dataBase->doesUserExist(username);

        if (userExists == static_cast<int>(DatabaseResult::USER_EXISTS))
        {
            std::cerr << "LoginManager: [ERROR]: User is already exists" << std::endl;
            return SignUpStatus::USER_ALREADY_EXISTS;
        }

        int dataBaseResult = dataBase->addNewUser(username, password, email);

        return (dataBaseResult == static_cast<int>(DatabaseResult::DATABASE_SUCCESS) ? SignUpStatus::SUCCESS : SignUpStatus::SIGNUP_ERROR);
    }

    return SignUpStatus::SIGNUP_ERROR;
}

LoginStatus LoginManager::login(const std::string& username, const std::string& password)
{
    std::cout << "Attempting to login user with username: " << username << " with password: " << password << std::endl;
    if (isUserAlreadyLogged(username))
    {
        std::cerr << "LoginManager: [ERROR]: User is already logged in!" << std::endl;
        return LoginStatus::USER_ALREADY_LOGGED_IN;
    }

    if (auto dataBase = m_dataBase.lock())
    {
        int userExists = dataBase->doesUserExist(username);

        if (userExists == static_cast<int>(DatabaseResult::USER_NOT_FOUND))
        {
            std::cerr << "LoginManager: [ERROR]: User doesn't exist!" << std::endl;
            return LoginStatus::USER_NOT_EXISTS;
        }

        int passwordState = dataBase->doesPasswordMatch(username, password);

        if (passwordState == static_cast<int>(DatabaseResult::PASSWORD_MATCH))
        {
            std::cout << "User sucessfully logged in!" << std::endl;
            std::lock_guard<std::mutex> lock(this->m_loggedUsersMutex);
            this->m_loggedUsers.push_back(LoggedUser(username));

            return LoginStatus::SUCCESS;
        }

        std::cerr << "LoginManager: [ERROR]: Incorrect password!" << std::endl;
        return LoginStatus::DISMATCHING_PASSWORD;
    }

    std::cerr << "LoginManager: [ERROR]: Error with database!" << std::endl;
    return LoginStatus::LOGIN_ERROR;
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
        std::cout << "Logging out user with username: "  << username << std::endl;
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

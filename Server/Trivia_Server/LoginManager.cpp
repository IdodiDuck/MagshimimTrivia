#include "LoginManager.h"
#include "SqliteDataBase.h"

#include <algorithm>
#include <iostream>

#include "ServerException.h"

LoginManager::LoginManager(std::weak_ptr<IDatabase> dataBase): m_dataBase(dataBase), m_isDbValid(false)
{
    if (auto db = m_dataBase.lock())
    {
        m_isDbValid = db->open();

        if (!m_isDbValid)
        {
            throw ServerException("[ERROR] Failed to open database in constructor.");
        }
    }

    else
    {
        throw ServerException("LoginManager: [ERROR] Database pointer expired.");
    }
}

LoginManager::~LoginManager()
{
    this->m_loggedUsers.clear();
}

SignUpStatus LoginManager::signUp(const std::string& username, const std::string& password, const std::string& email)
{
    std::cout << "Attempting to signup user with username: " << username << std::endl;

    if (!m_isDbValid)
    {
        std::cerr << "LoginManager: [ERROR] Cannot sign up. Database not available.\n";
        return SignUpStatus::SIGNUP_ERROR;
    }

    if (auto dataBase = m_dataBase.lock())
    {
        int result = dataBase->addNewUser(username, password, email);
        return (result == static_cast<int>(DatabaseResult::DATABASE_SUCCESS)) ? SignUpStatus::SUCCESS : SignUpStatus::SIGNUP_ERROR;
    }

    return SignUpStatus::SIGNUP_ERROR;
}

LoginStatus LoginManager::login(const std::string& username, const std::string& password)
{
    std::cout << "Attempting to login user with username: " << username << std::endl;

    if (isUserAlreadyLogged(username))
    {
        std::cerr << "LoginManager: [ERROR] User is already logged in.\n";
        return LoginStatus::USER_ALREADY_LOGGED_IN;
    }

    if (!m_isDbValid)
    {
        std::cerr << "LoginManager: [ERROR] Cannot login. Database not available.\n";
        return LoginStatus::LOGIN_ERROR;
    }

    if (auto dataBase = m_dataBase.lock())
    {
        if (dataBase->doesUserExist(username) == static_cast<int>(DatabaseResult::USER_NOT_FOUND))
        {
            std::cerr << "LoginManager: [ERROR] User doesn't exist.\n";
            return LoginStatus::USER_NOT_EXISTS;
        }

        if (dataBase->doesPasswordMatch(username, password) == static_cast<int>(DatabaseResult::PASSWORD_MATCH))
        {
            std::cout << "User successfully logged in.\n";
            std::lock_guard<std::mutex> lock(this->m_loggedUsersMutex);
            this->m_loggedUsers.emplace_back(username);
            return LoginStatus::SUCCESS;
        }

        std::cerr << "LoginManager: [ERROR] Incorrect password.\n";
        return LoginStatus::DISMATCHING_PASSWORD;
    }

    std::cerr << "LoginManager: [ERROR] Database reference expired.\n";
    return LoginStatus::LOGIN_ERROR;
}

void LoginManager::logOut(const std::string& username)
{
    std::lock_guard<std::mutex> lock(this->m_loggedUsersMutex);

    auto loggedOutUserIt = std::find_if(this->m_loggedUsers.begin(), this->m_loggedUsers.end(),
        [&](const LoggedUser& user) { return user.getUserName() == username; });

    if (loggedOutUserIt != m_loggedUsers.end())
    {
        std::cout << "Logging out user: " << username << std::endl;
        this->m_loggedUsers.erase(loggedOutUserIt);
    }
}

bool LoginManager::isUserAlreadyLogged(const std::string& username)
{
    std::lock_guard<std::mutex> lock(this->m_loggedUsersMutex);

    return std::any_of(this->m_loggedUsers.begin(), this->m_loggedUsers.end(),
        [&](const LoggedUser& user) { return user.getUserName() == username; });
}

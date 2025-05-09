#pragma once

#include "LoggedUser.h"
#include "IDatabase.h"

#include <vector>
#include <memory>
#include <mutex>

enum class SignUpStatus
{
    SIGNUP_ERROR = -1,
    SUCCESS,
    USER_ALREADY_EXISTS,
};

enum class LoginStatus
{
    LOGIN_ERROR = -1,
    SUCCESS,
    DISMATCHING_PASSWORD,
    USER_NOT_EXISTS,
    USER_ALREADY_LOGGED_IN
};

class LoginManager
{
public:

    LoginManager(std::weak_ptr<IDatabase> dataBase);
    ~LoginManager();

    SignUpStatus signUp(const std::string& username, const std::string& password, const std::string& email);
    LoginStatus login(const std::string& username, const std::string& password);
    void logOut(const std::string& username);


private:
    std::weak_ptr<IDatabase> m_dataBase;
    std::vector<LoggedUser> m_loggedUsers;
    std::mutex m_loggedUsersMutex;
    bool m_isDbValid;

    bool isUserAlreadyLogged(const std::string& username);

};


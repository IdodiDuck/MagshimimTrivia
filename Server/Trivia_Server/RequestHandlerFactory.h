#pragma once

#include "LoginManager.h"
#include "MenuRequestHandler.h"

class LoginRequestHandler;

class RequestHandlerFactory
{

public:
	RequestHandlerFactory(std::weak_ptr<IDatabase> database);

	std::unique_ptr<LoginRequestHandler> createLoginRequestHandler();
	LoginManager& getLoginManager();

private:
	LoginManager m_loginManager;
	std::weak_ptr<IDatabase> m_database;

};

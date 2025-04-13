#pragma once

#include "LoginManager.h"
#include "LoginRequestHandler.h"
#include "MenuRequestHandler.h"

class RequestHandlerFactory
{

public:
	std::unique_ptr<LoginRequestHandler> createLoginRequestHandler();
	LoginManager& getLoginManager();

private:
	LoginManager m_loginManager;
	std::weak_ptr<IDatabase> m_database;

};

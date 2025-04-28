#pragma once

#include "LoginManager.h"
#include "MenuRequestHandler.h"
#include "RoomManager.h"
#include "StatisticsManager.h"

class LoginRequestHandler;
class MenuRequestHandler;

class RequestHandlerFactory
{

public:
	RequestHandlerFactory(std::weak_ptr<IDatabase> database);

	std::unique_ptr<LoginRequestHandler> createLoginRequestHandler();
	std::unique_ptr<MenuRequestHandler> createMenuRequestHandler(const LoggedUser& user);
	LoginManager& getLoginManager();

private:
	LoginManager m_loginManager;
	std::weak_ptr<IDatabase> m_database;
	RoomManager m_roomManager;
	StatisticsManager m_StatisticsManager;

};

#pragma once

#include "LoginRequestHandler.h"
#include "MenuRequestHandler.h"
#include "RoomAdminRequestHandler.h"
#include "RoomMemberRequestHandler.h"
#include "GameRequestHandler.h"

#include "LoginManager.h"
#include "RoomManager.h"
#include "StatisticsManager.h"
#include "GameManager.h"

#include <memory>
#include <vector>

class RequestHandlerFactory : public std::enable_shared_from_this<RequestHandlerFactory>
{

public:
	// C'tor - 
	RequestHandlerFactory(std::weak_ptr<IDatabase> database);

	// Handlers Getters - 
	std::unique_ptr<IRequestHandler> createLoginRequestHandler();
	std::unique_ptr<IRequestHandler> createMenuRequestHandler(const LoggedUser& user);
	std::unique_ptr<IRequestHandler> createRoomAdminRequestHandler(const LoggedUser& user, int roomId);
	std::unique_ptr<IRequestHandler> createRoomMemberRequestHandler(const LoggedUser& user, int roomId);
	std::unique_ptr<IRequestHandler> createGameRequestHandler(const LoggedUser& user, Game& game);

	// Managers Getters - 
	LoginManager& getLoginManager();
	StatisticsManager& getStatisticsManager();
	RoomManager& getRoomManager();
	GameManager& getGameManager();

private:
	// Attributes - 
	std::weak_ptr<IDatabase> m_database;

	// Managers - 
	LoginManager m_loginManager;
	RoomManager m_roomManager;
	StatisticsManager m_StatisticsManager;
	GameManager m_gameManager;

};

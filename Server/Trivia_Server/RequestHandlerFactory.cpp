#include "RequestHandlerFactory.h"

RequestHandlerFactory::RequestHandlerFactory(std::weak_ptr<IDatabase> database):
	m_database(database), m_loginManager(database), m_roomManager(), m_StatisticsManager(database), m_gameManager(database)
{

}

std::unique_ptr<IRequestHandler> RequestHandlerFactory::createLoginRequestHandler()
{
	return std::make_unique<LoginRequestHandler>(shared_from_this());
}

std::unique_ptr<IRequestHandler> RequestHandlerFactory::createMenuRequestHandler(const LoggedUser& user)
{
	return std::make_unique<MenuRequestHandler>(shared_from_this(), user);
}

std::unique_ptr<IRequestHandler> RequestHandlerFactory::createRoomAdminRequestHandler(const LoggedUser& user, const int roomId)
{
	auto desiredRoom = this->m_roomManager.getRoom(roomId);

	if (!desiredRoom.has_value())
	{
		throw std::runtime_error("Room with given ID does not exist.");
	}

	return std::make_unique<RoomAdminRequestHandler>(shared_from_this(), this->m_roomManager, user, desiredRoom.value());
}

std::unique_ptr<IRequestHandler> RequestHandlerFactory::createRoomMemberRequestHandler(const LoggedUser& user, const int roomId)
{
	auto desiredRoom = this->m_roomManager.getRoom(roomId);

	if (!desiredRoom.has_value())
	{
		throw std::runtime_error("Room with given ID does not exist.");
	}

	return std::make_unique<RoomMemberRequestHandler>(shared_from_this(), this->m_roomManager, user, desiredRoom.value());
}

std::unique_ptr<IRequestHandler> RequestHandlerFactory::createGameRequestHandler(const LoggedUser& user, Game& game)
{
	return std::make_unique<GameRequestHandler>(shared_from_this(), user, this->m_gameManager, game);
}

LoginManager& RequestHandlerFactory::getLoginManager()
{
	return this->m_loginManager;
}

StatisticsManager& RequestHandlerFactory::getStatisticsManager()
{
	return this->m_StatisticsManager;
}

RoomManager& RequestHandlerFactory::getRoomManager()
{
	return this->m_roomManager;
}

GameManager& RequestHandlerFactory::getGameManager()
{
	return this->m_gameManager;
}

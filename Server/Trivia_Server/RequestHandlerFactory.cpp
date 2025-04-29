#include "RequestHandlerFactory.h"

RequestHandlerFactory::RequestHandlerFactory(std::weak_ptr<IDatabase> database):
	m_database(database), m_loginManager(database), m_roomManager(), m_StatisticsManager(database)
{

}

std::unique_ptr<LoginRequestHandler> RequestHandlerFactory::createLoginRequestHandler()
{
	return std::make_unique<LoginRequestHandler>(shared_from_this());
}

std::unique_ptr<MenuRequestHandler> RequestHandlerFactory::createMenuRequestHandler(const LoggedUser& user)
{
	return std::make_unique<MenuRequestHandler>(shared_from_this(), user);
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


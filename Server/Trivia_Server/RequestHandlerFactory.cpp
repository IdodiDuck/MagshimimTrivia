#include "RequestHandlerFactory.h"

#include "LoginRequestHandler.h"

RequestHandlerFactory::RequestHandlerFactory(std::weak_ptr<IDatabase> database):
	m_database(database), m_loginManager(database)
{

}

std::unique_ptr<LoginRequestHandler> RequestHandlerFactory::createLoginRequestHandler()
{
	return std::make_unique<LoginRequestHandler>(*this);
}

std::unique_ptr<MenuRequestHandler> RequestHandlerFactory::createMenuRequestHanndler()
{
	return std::unique_ptr<MenuRequestHandler>(); // Will be changed...
}

LoginManager& RequestHandlerFactory::getLoginManager()
{
	return this->m_loginManager;
}


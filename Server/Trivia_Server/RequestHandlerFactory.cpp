#include "RequestHandlerFactory.h"

#include "LoginRequestHandler.h"
#include "MenuRequestHandler.h"

RequestHandlerFactory::RequestHandlerFactory(std::weak_ptr<IDatabase> database):
	m_database(database), m_loginManager(database)
{

}

std::unique_ptr<LoginRequestHandler> RequestHandlerFactory::createLoginRequestHandler()
{
	return std::make_unique<LoginRequestHandler>(*this);
}

std::unique_ptr<MenuRequestHandler> RequestHandlerFactory::createMenuRequestHandler()
{
	return std::make_unique<MenuRequestHandler>(*this);
}

LoginManager& RequestHandlerFactory::getLoginManager()
{
	return this->m_loginManager;
}


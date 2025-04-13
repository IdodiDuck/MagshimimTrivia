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

LoginManager& RequestHandlerFactory::getLoginManager()
{
	return this->m_loginManager;
}


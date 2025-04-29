#pragma once

// Local Libraries Importation - 
#include "Communicator.h"
#include "RequestHandlerFactory.h"

#include <memory>

class Server
{

public:
	Server();
	~Server();

	void run();

private:
	// Attributes - 
	std::shared_ptr<IDatabase> m_database;
	std::shared_ptr<RequestHandlerFactory> m_handlerFactory;
	Communicator m_communicator;
};

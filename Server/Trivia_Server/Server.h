#pragma once

// Local Libraries Importation - 
#include "Communicator.h"
#include "RequestHandlerFactory.h"

#include <memory>

class Server
{

public:
	void run();

private:
	// Attributes - 
	Communicator m_communicator;
	std::weak_ptr<IDatabase> m_database;
	RequestHandlerFactory m_handlerFactory;
};

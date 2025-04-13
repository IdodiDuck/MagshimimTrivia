#pragma once

// Local Libraries Importation - 
#include "Communicator.h"
#include "RequestHandlerFactory.h"

class Server
{

public:
	void run();

private:
	// Attributes - 
	Communicator m_communicator;
	IDatabase* m_database;
	RequestHandlerFactory m_handlerFactory;
};

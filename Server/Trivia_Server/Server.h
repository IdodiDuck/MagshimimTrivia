#pragma once

// Local Libraries Importation - 
#include "Constants.h"
#include "Communicator.h"

class Server
{

public:
	void run();

private:
	// Attributes - 
	Communicator m_communicator;
};

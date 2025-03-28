#pragma once

// Local Libraries Importation - 
#include "Communicator.h"

class Server
{

public:
	void run();

private:
	// Attributes - 
	Communicator m_communicator;
};

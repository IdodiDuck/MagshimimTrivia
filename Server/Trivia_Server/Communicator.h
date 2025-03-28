#pragma once

// Network Build Importations - 
#include <WinSock2.h>

// STD Libraries Importations - 
#include <thread>
#include <map>
#include <memory>

// Local Files Importations - 
#include "IRequestHandler.h"

class Communicator
{

public:

	~Communicator();
	void startHandleRequests();

private:
	// Attributes - 
	SOCKET m_serverSocket;
	std::map <SOCKET, std::unique_ptr<IRequestHandler>> m_clients;

	// Private Methods - 
	void bindAndListen();
	void handleNewClient(SOCKET clientSocket);

	// Support Methods -
	void sendHelloToClient(SOCKET clientSocket);
	void receiveHelloFromClient(SOCKET clientSocket);
	void disconnectClient(SOCKET removedSocket);
	bool doesClientExists(const SOCKET clientSocket);
};


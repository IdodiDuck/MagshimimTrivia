#pragma once

// Network Build Importations - 
#include <WinSock2.h>

// STD Libraries Importations - 
#include <thread>
#include <map>
#include <memory>
#include <string>

// Local Files Importations - 
#include "IRequestHandler.h"
#include "RequestHandlerFactory.h"

class Communicator
{

public:
	Communicator(RequestHandlerFactory& handlerFactory);
	~Communicator();
	void startHandleRequests();

private:
	// Attributes - 
	SOCKET m_serverSocket;
	std::map <SOCKET, std::unique_ptr<IRequestHandler>> m_clients;
	RequestHandlerFactory& m_handlerFactory;

	// Private Methods - 
	void bindAndListen();
	void handleNewClient(SOCKET clientSocket);

	// Support Methods -
	void disconnectClient(SOCKET removedSocket);
	bool doesClientExists(const SOCKET clientSocket);
	RequestInfo parseClientRequest(const SOCKET clientSocket);
	void sendErrorResponse(SOCKET clientSocket, const std::string& errorMessage);
	void sendClientResponse(SOCKET clientSocket, const std::vector<unsigned char>& response);
};


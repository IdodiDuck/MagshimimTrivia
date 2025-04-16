#pragma once

// Network Build Importations - 
#include <WinSock2.h>

// STD Libraries Importations - 
#include <thread>
#include <unordered_map>
#include <memory>
#include <string>
#include <atomic>
#include <mutex>

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
	std::unordered_map <SOCKET, std::unique_ptr<IRequestHandler>> m_clients;
	RequestHandlerFactory& m_handlerFactory;
	std::atomic<bool> m_isRunning; // Used to synchronize between the threads if the server is still active
	std::mutex m_clientsMutex;

	// Private Methods - 
	void bindAndListen();
	void handleNewClient(SOCKET clientSocket);

	// Support Methods -
	void disconnectClient(SOCKET removedSocket);
	bool doesClientExists(const SOCKET clientSocket);

	RequestInfo parseClientRequest(const SOCKET clientSocket);
	RequestInfo safeParseClientRequest(SOCKET clientSocket);

	void processClientRequest(SOCKET clientSocket, RequestInfo& info);
	void sendErrorResponse(SOCKET clientSocket, const std::string& errorMessage);
	void sendClientResponse(SOCKET clientSocket, const std::vector<unsigned char>& response);

	friend class Server; // Only server can call stop communicating
	void stopCommunicating();
};


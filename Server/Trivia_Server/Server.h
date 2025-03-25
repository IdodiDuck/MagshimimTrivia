#pragma once

// Network Build Importations - 
#include <WinSock2.h>
#include <Windows.h>

// STD Libraries Importations - 
#include <thread>
#include <unordered_map>

// Local Files Importations - 
#include "IRequestHandler.h"
#include "Constants.h"

class Server
{

public:

	Server();
	~Server();
	void run(const int port);

private:

	// Server Methods - 
	void connectClients();
	void acceptClient();
	void clientHandler(const SOCKET clientSocket);
	
	void introduceClient(SOCKET clientSocket);

	// Support Methods - 

	// Attributes - 
	SOCKET _serverSocket;

	// Containers Attributes - 
	std::unordered_map<SOCKET, IRequestHandler*> _servedClients;

};

#include "Server.h"
#include "LoginRequestHandler.h"

#include <iostream>
#include <algorithm>

Server::Server()
{

	// this server use TCP. that why SOCK_STREAM & IPPROTO_TCP
	// if the server use UDP we will use: SOCK_DGRAM & IPPROTO_UDP
	this->_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (_serverSocket == INVALID_SOCKET)
	{
		throw std::exception(__FUNCTION__ " - socket");
	}
}

Server::~Server()
{
	try
	{
		for (const auto& currentServedClient : this->_servedClients)
		{
			// Removing each client by it's socket identifier
			removeClient(currentServedClient.first);
		}

		// the only use of the destructor should be for freeing 
		// resources that was allocated in the constructor
		closesocket(this->_serverSocket);
		WSACleanup();
	}

	catch (...) {}
}

void Server::run(const int port)
{

	struct sockaddr_in sa = { 0 };

	sa.sin_port = htons(port); // port that server will listen for
	sa.sin_family = AF_INET;   // must be AF_INET
	sa.sin_addr.s_addr = INADDR_ANY;    // when there are few ip's for the machine. We will use always "INADDR_ANY"

	// Connects between the socket and the configuration (port and etc..)
	if (bind(_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
	{
		throw std::exception(__FUNCTION__ " - bind");
	}

	// Start listening for incoming requests of clients
	if (listen(_serverSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		throw std::exception(__FUNCTION__ " - listen");
	}

	std::cout << "Listening on port " << port << std::endl << std::endl;

	std::thread connectorThread(&Server::connectClients, this); // Activating clients connection thread
	connectorThread.detach();

	std::string userConsoleInput = "";
	while (true)
	{
		std::cin >> userConsoleInput;

		// If console input was to exit then we eventually finish execution of main thread (all destructors are called)
		if (userConsoleInput == EXIT_SERVER)
		{
			std::cout << "Server: Closing server as requested...\n";
			return;
		}

		system("cls");
		std::cout << "Listening on port " << port << std::endl << std::endl;
		std::cout << "CONNECT_CLIENTS: Waiting for connections...\n" << std::endl;
	}

}

void Server::connectClients()
{
	std::cout << "CONNECT_CLIENTS: Waiting for connections...\n" << std::endl;
	while (true)
	{
		try
		{
			acceptClient();
		}

		// If no clients were connected then we just keep waiting
		catch (...) {}
	}

}

void Server::acceptClient()
{
	// this accepts the client and create a specific socket from server to this client
	// the process will not continue until a client connects to the server
	SOCKET clientSocket = accept(this->_serverSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET)
	{
		throw std::exception(__FUNCTION__);
	}

	this->_servedClients[clientSocket] = new LoginRequestHandler(); // Adding the client to the map
	std::thread currentClient(&Server::clientHandler, this, clientSocket);
	currentClient.detach();
}

void Server::clientHandler(const SOCKET clientSocket)
{
	try
	{
		introduceClient(clientSocket);
		closesocket(clientSocket);
	}

	// If at any stage there is an error we remove the user
	catch (const std::exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		closesocket(clientSocket);
	}

}

void Server::introduceClient(SOCKET clientSocket)
{
	const int INTRODUCTION_MESSAGE_LENGTH = 6, DEFAULT_RECV_FLAGS = 0;
	const char* data = "Hello";

	std::cout << "Sending to client socket " << clientSocket << " hello message..." << std::endl;
	if (send(clientSocket, data, INTRODUCTION_MESSAGE_LENGTH, 0) == INVALID_SOCKET)
	{
		throw std::exception("Error while sending message to client");
	}

	char introductionBuffer[BUFFER_SIZE] = { 0 };

	int clientResponse = 0;

	// Server keeps waiting for hello from the client socket
	while (true)
	{
		clientResponse = recv(clientSocket, introductionBuffer, INTRODUCTION_MESSAGE_LENGTH, DEFAULT_RECV_FLAGS);
		
		if (clientResponse == SOCKET_ERROR)
		{
			std::string errorDescription = "Error while receiving from socket: ";
			errorDescription += std::to_string(clientSocket);
			throw std::exception(errorDescription.c_str());
		}

		introductionBuffer[clientResponse] = 0;

		std::string receivedMessage(introductionBuffer);


		if (receivedMessage == data)
		{
			std::cout << "Received from the client: " << receivedMessage << std::endl;
			break;
		}

	}

	std::cout << "Received from client: " << introductionBuffer << std::endl;
	std::cout << "Disconnecting client: " << clientSocket << std::endl;
	removeClient(clientSocket);
}

void Server::removeClient(SOCKET removedSocket)
{
	// Finding the iterator of the client we remove from map
	auto removedClientIt = std::find_if(_servedClients.begin(), _servedClients.end(),
	[removedSocket](const std::pair<SOCKET, IRequestHandler*>& client) 
	{
		return client.first == removedSocket;
	});

	if (removedClientIt != this->_servedClients.end())
	{
		// Freeing IRequestHandler pointer allocated memory (second value in client's map pairs)
		IRequestHandler* currentHandle = removedClientIt->second;
		delete currentHandle;
		currentHandle = nullptr;

		this->_servedClients.erase(removedClientIt);
		return;
	}

	std::cerr << "Failed to disconnect client with socket: " << removedClientIt->first << std::endl;
}

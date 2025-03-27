#include "Communicator.h"
#include "Constants.h"
#include "LoginRequestHandler.h"

#include <iostream>

Communicator::~Communicator()
{
    for (auto& client : this->m_clients)
    {
        disconnectClient(client.first); // Closing remained clients that weren't disconnected unexpectedly
    }

    // Closing the server socket (if it isn't closed yet)
    if (m_serverSocket != INVALID_SOCKET)
    {
        closesocket(m_serverSocket);
    }

    WSACleanup();
    std::cout << "Communicator shut down successfully." << std::endl;
}

void Communicator::startHandleRequests()
{
    bindAndListen();

    while (true)
    {
        SOCKET clientSocket = accept(m_serverSocket, NULL, NULL);

        // If a valid client would like to have a connection we accept him and handle his communication
        if (clientSocket != INVALID_SOCKET)
        {
            if (!doesClientExists(clientSocket))
            {
                m_clients[clientSocket] = std::make_shared<IRequestHandler>();
            }

            std::thread(&Communicator::handleNewClient, this, clientSocket).detach();
        }
    }
}

void Communicator::bindAndListen()
{
    struct sockaddr_in serverAddr = { 0 };
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(m_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        throw std::exception("Error: Failed to bind socket.\n");
    }

    if (listen(m_serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        throw std::exception("Error: Failed to listen on socket.\n");
    }

    std::cout << "Server listening on port " << SERVER_PORT << std::endl << std::endl;
}

void Communicator::handleNewClient(SOCKET clientSocket)
{
    try
	{
		sendHelloToClient(clientSocket);
		receiveHelloFromClient(clientSocket);
		disconnectClient(clientSocket);
	}

	catch (const std::exception& e)
	{
		std::cerr << "Error handling client: " << e.what() << std::endl;
		disconnectClient(clientSocket);
	}
}

void Communicator::sendHelloToClient(SOCKET clientSocket)
{
    const char* data = "Hello";
    const int INTRODUCTION_MESSAGE_LENGTH = 6;

    std::cout << "Sending to client socket " << clientSocket << " hello message..." << std::endl;

    if (send(clientSocket, data, INTRODUCTION_MESSAGE_LENGTH, 0) == SOCKET_ERROR)
    {
        throw std::exception("Error while sending message to client\n");
    }
}

void Communicator::receiveHelloFromClient(SOCKET clientSocket)
{
    const int INTRODUCTION_MESSAGE_LENGTH = 6;
    const int DEFAULT_RECV_FLAGS = 0;

    char introductionBuffer[BUFFER_SIZE] = { 0 };
    int clientResponse = 0;
    const char* data = "Hello";

    // Server keeps waiting for hello from the client socket
    while (true)
    {
        clientResponse = recv(clientSocket, introductionBuffer, INTRODUCTION_MESSAGE_LENGTH, DEFAULT_RECV_FLAGS);

        if (clientResponse == SOCKET_ERROR)
        {
            std::string errorDescription = "Error while receiving from socket: ";
            errorDescription += std::to_string(clientSocket) + "\n";
            throw std::exception(errorDescription.c_str());
        }

        introductionBuffer[clientResponse] = 0;  // Ensure null termination
        std::string receivedMessage(introductionBuffer);

        if (receivedMessage == data)
        {
            std::cout << "Received from the client: " << receivedMessage << std::endl;
            return;
        }
    }
}

void Communicator::disconnectClient(SOCKET removedSocket)
{
    std::cout << "Disconnecting client: " << removedSocket << std::endl << std::endl;

    auto removedSocketIt = m_clients.find(removedSocket);
    
    // If we found the socket we remove it from the map and after we close it
    if (removedSocketIt != m_clients.end())
    {
        m_clients.erase(removedSocketIt);
        closesocket(removedSocket);

        return;
    }

    std::cerr << "Failed to disconnect client" << std::endl;
}

bool Communicator::doesClientExists(const SOCKET clientSocket)
{
    auto clientIt = m_clients.find(clientSocket);
    return (clientIt != m_clients.cend());
}

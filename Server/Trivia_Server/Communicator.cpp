#include "Communicator.h"

#include "Constants.h"
#include "LoginRequestHandler.h"
#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"
#include "SocketHelper.h"

#include <iostream>
#include <ctime>

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
                m_clients[clientSocket] = std::make_unique<LoginRequestHandler>();
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
        RequestInfo info = parseClientRequest(clientSocket);
        RequestResult res = m_clients[clientSocket]->handleRequest(info);
        m_clients[clientSocket] = std::move(res.newHandler);
        std::vector<unsigned char> buffer = res.response;
        std::string response = std::string(buffer.cbegin(), buffer.cend());
        SocketHelper::sendData(clientSocket, response);
		
		disconnectClient(clientSocket);
	}

	catch (const std::exception& e)
	{
		std::cerr << "Error handling client: " << e.what() << std::endl;
		disconnectClient(clientSocket);
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

RequestInfo Communicator::parseClientRequest(const SOCKET clientSocket)
{
    const int MESSAGE_CODE_BYTE = 1;
    const int DEFAULT_RECV_FLAGS = 0;
    const int MESSAGE_LENGTH_BYTE = 4;

    int msgCode = 0, msgLen = 0;

    msgCode = SocketHelper::getIntPartFromSocket(clientSocket, MESSAGE_CODE_BYTE);
    msgLen = SocketHelper::getIntPartFromSocket(clientSocket, DEFAULT_RECV_FLAGS);

    std::vector<unsigned char> recievedData = SocketHelper::getData(clientSocket, msgLen);

    return { msgCode, std::time(nullptr), recievedData }; 
}

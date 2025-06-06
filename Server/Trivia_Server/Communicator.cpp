#include "Communicator.h"

#include "Constants.h"
#include "JsonResponsePacketSerializer.h"
#include "SocketHelper.h"
#include "RequestHandlerFactory.h"

#include "ClientConnectionException.h"

#include <iostream>
#include <ctime>

Communicator::Communicator(std::weak_ptr<RequestHandlerFactory> handlerFactory): m_handlerFactory(handlerFactory), m_isRunning(true)
{
    this->m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (m_serverSocket == INVALID_SOCKET)
    {
        throw std::runtime_error("Error: Failed to create server socket.");
    }
}

Communicator::~Communicator()
{
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    for (auto& client : this->m_clients)
    {
        disconnectClient(client.first); // Closing remained clients that weren't disconnected unexpectedly
    }

    // Closing the server socket (if it isn't closed yet)
    if (this->m_serverSocket != INVALID_SOCKET)
    {
        closesocket(this->m_serverSocket);
    }

    WSACleanup();
    std::cout << "Communicator shut down successfully." << std::endl;
}

void Communicator::startHandleRequests()
{
    bindAndListen();

    while (this->m_isRunning)
    {
        SOCKET clientSocket = accept(this->m_serverSocket, NULL, NULL);

        // If a valid client would like to have a connection we accept him and handle his communication
        if (clientSocket != INVALID_SOCKET)
        {
            if (!doesClientExists(clientSocket))
            {
                m_clients[clientSocket] = this->m_handlerFactory.lock()->createLoginRequestHandler();
                std::thread(&Communicator::handleNewClient, this, clientSocket).detach();
            }
        }
    }
}

void Communicator::bindAndListen()
{
    struct sockaddr_in serverAddr = { 0 };
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

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
    RequestInfo info;

    while (true)
    {
        try
        {
            info = parseClientRequest(clientSocket);

            if (m_clients[clientSocket]->isRequestRelevant(info))
            {
                processClientRequest(clientSocket, info);
            }

            else if (!info.buffer.empty())
            {
                std::cout << "Invalid Empty Request" << std::endl;
                sendErrorResponse(clientSocket, "Error: Invalid type of request.");
            }
        }

        catch (const ClientConnectionException& e)
        {
            std::cerr << "Error handling client: " << e.what() << std::endl;
            disconnectClient(clientSocket);
            break;
        }

        catch (const std::exception& e)
        {
            std::cerr << "Error handling client: " << e.what() << std::endl;
            disconnectClient(clientSocket);
            break;
        }
    }
}

void Communicator::disconnectClient(SOCKET removedSocket)
{
    try
    {
        if (doesClientExists(removedSocket))
        {
            m_clients[removedSocket]->handleDisconnection();
            m_clients.erase(removedSocket);
            closesocket(removedSocket);

            std::cout << std::endl;
        }
    }

    catch (...)
    {
        std::cerr << "Failed to disconnect client with socket: " << removedSocket << std::endl;
    }
}

bool Communicator::doesClientExists(const SOCKET clientSocket)
{
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    auto clientIt = m_clients.find(clientSocket);
    return (clientIt != m_clients.cend());
}

RequestInfo Communicator::parseClientRequest(const SOCKET clientSocket)
{
    int requestCode = 0, requestLength = 0;

    auto requestCodeOpt = SocketHelper::getRequestCode(clientSocket);
    if (!requestCodeOpt.has_value())  // If there's an error retrieving the request code
    {
        throw ClientConnectionException("Error: Failed to read request code from socket.");
    }

    requestCode = requestCodeOpt.value();

    auto requestLengthOpt = SocketHelper::getRequestLength(clientSocket);
    if (!requestLengthOpt.has_value())  // If there's an error retrieving the request length
    {
        throw ClientConnectionException("Error: Failed to read request length from socket.");
    }

    requestLength = requestLengthOpt.value();

    auto requestJSONDataOpt = SocketHelper::getData(clientSocket, requestLength);
    if (!requestJSONDataOpt.has_value())  // If there's an error receiving the JSON data
    {
        throw ClientConnectionException("Error: Failed to read JSON data from socket.");
    }

    std::vector<unsigned char> requestJSONData = requestJSONDataOpt.value();

    return { requestCode, std::time(nullptr), requestJSONData };
}

void Communicator::processClientRequest(SOCKET clientSocket, RequestInfo& info)
{
    RequestResult handlingResult = m_clients[clientSocket]->handleRequest(info);
    m_clients[clientSocket] = std::move(handlingResult.newHandler);
    std::vector<unsigned char> buffer = handlingResult.response;
    std::string response = (buffer.empty() ? "" : std::string(buffer.begin(), buffer.end()));

    sendClientResponse(clientSocket, buffer);
}

void Communicator::sendErrorResponse(SOCKET clientSocket, const std::string& errorMessage)
{
    ErrorResponse errResponse;
    errResponse.message = errorMessage;

    std::vector<unsigned char> error = JsonResponsePacketSerializer::serializeResponse(errResponse);

    SocketHelper::sendData(clientSocket, error);
}

void Communicator::sendClientResponse(SOCKET clientSocket, const std::vector<unsigned char>& response)
{
    std::cout << "Sending back the response to the client" << std::endl;

    SocketHelper::sendData(clientSocket, response);
    std::cout << std::endl;
}

void Communicator::stopCommunicating()
{
    this->m_isRunning = false;

    // Closing the server socket in order to release accept so the communicator and server can shut down
    if (this->m_serverSocket != INVALID_SOCKET)
    {
        closesocket(this->m_serverSocket);
    }

    WSACleanup();
}

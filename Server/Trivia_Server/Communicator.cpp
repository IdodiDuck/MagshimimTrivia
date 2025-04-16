#include "Communicator.h"

#include "Constants.h"
#include "LoginRequestHandler.h"
#include "JsonResponsePacketSerializer.h"
#include "SocketHelper.h"
#include "RequestHandlerFactory.h"

#include <iostream>
#include <ctime>

Communicator::Communicator(RequestHandlerFactory& handlerFactory) : m_handlerFactory(handlerFactory), m_isRunning(true)
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
                m_clients[clientSocket] = this->m_handlerFactory.createLoginRequestHandler();
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
    const std::string EMPTY_CONTENT = "";
    const int EMPTY = 0;
    RequestInfo info;

    while (true)
    {
        try
        {
            try
            {
                info = parseClientRequest(clientSocket);
            }

            catch (const std::exception& e)
            {
                std::cerr << "[PARSER ERROR]: " << e.what() << std::endl;
                disconnectClient(clientSocket);
                break;
            }

            if (m_clients[clientSocket]->isRequestRelevant(info))
            {
                std::cout << "Handling the request, getting it's results..." << std::endl;
                RequestResult res = m_clients[clientSocket]->handleRequest(info);

                std::cout << "Giving the new handler to the client..." << std::endl;
                m_clients[clientSocket] = std::move(res.newHandler);

                std::cout << "Constructing response to be sent..." << std::endl;
                std::vector<unsigned char> buffer = res.response;
                std::string response = (buffer.size() == EMPTY ? "" : std::string(buffer.cbegin(), buffer.cend()));

                // Sending back to the client a response, then we disconnect him
                if (response != EMPTY_CONTENT)
                {
                    sendClientResponse(clientSocket, buffer);
                    break;
                }
            }

            // In-case the client has sent a request with invalid code
            if (info.buffer.size() != EMPTY)
            {
                std::cout << "Client requested invalid type of request..." << std::endl;
                sendErrorResponse(clientSocket, "Error: Invalid type of request.");
                break;
            }

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
            m_clients.erase(removedSocket);
            closesocket(removedSocket);
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

    // Extracting the type of request (code) and the length of the JSON sent buffer by the protocol

    auto requestCodeOpt = SocketHelper::getRequestCode(clientSocket);
    if (!requestCodeOpt.has_value())  // If there's an error retrieving the request code
    {
        throw std::runtime_error("Error: Failed to read request code from socket.");
    }

    requestCode = requestCodeOpt.value();

    // Get the request length
    auto requestLengthOpt = SocketHelper::getRequestLength(clientSocket);

    if (!requestLengthOpt.has_value())  // If there's an error retrieving the request length
    {
        throw std::runtime_error("Error: Failed to read request length from socket.");
    }

    requestLength = requestLengthOpt.value();

    // Get the actual JSON data
    auto requestJSONDataOpt = SocketHelper::getData(clientSocket, requestLength);
    if (!requestJSONDataOpt.has_value())  // If there's an error receiving the JSON data
    {
        throw std::runtime_error("Error: Failed to read JSON data from socket.");
    }

    std::vector<unsigned char> requestJSONData = requestJSONDataOpt.value();

    // Return the parsed request info
    return { requestCode, std::time(nullptr), requestJSONData };
}

void Communicator::processClientRequest(SOCKET clientSocket, RequestInfo& info)
{
    const std::string EMPTY_CONTENT = "";

    std::cout << "Handling the request, getting its results..." << std::endl;
    RequestResult res = m_clients[clientSocket]->handleRequest(info);

    std::cout << "Giving the new handler to the client..." << std::endl;
    m_clients[clientSocket] = std::move(res.newHandler);

    std::cout << "Constructing response to be sent..." << std::endl;
    std::vector<unsigned char> buffer = res.response;
    std::string response = (buffer.empty() ? "" : std::string(buffer.begin(), buffer.end()));

    if (response != EMPTY_CONTENT)
    {
        sendClientResponse(clientSocket, buffer);
    }
}

void Communicator::sendErrorResponse(SOCKET clientSocket, const std::string& errorMessage)
{
    std::cout << "Client requested invalid type of request..." << std::endl;

    ErrorResponse errResponse;
    errResponse.message = errorMessage;

    std::vector<unsigned char> error = JsonResponsePacketSerializer::serializeResponse(errResponse);

    SocketHelper::sendData(clientSocket, error);
    disconnectClient(clientSocket);
}

void Communicator::sendClientResponse(SOCKET clientSocket, const std::vector<unsigned char>& response)
{
    std::cout << "Sending back the response to the client" << std::endl;

    SocketHelper::sendData(clientSocket, response);
    disconnectClient(clientSocket);
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

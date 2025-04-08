#include "Communicator.h"

#include "Constants.h"
#include "LoginRequestHandler.h"
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
    const std::string EMPTY_RESPONSE = "";
    const int EMPTY = 0;

    while (true)
    {
        try
        {
            RequestInfo info = parseClientRequest(clientSocket);

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
                if (response != EMPTY_RESPONSE)
                {
                    std::cout << "Sending back the response to the client" << std::endl;
                    SocketHelper::sendData(clientSocket, response);
                    disconnectClient(clientSocket);
                    break;
                }
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

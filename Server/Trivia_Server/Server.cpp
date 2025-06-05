#include "Server.h"

#include "SqliteDataBase.h"
#include "Constants.h"

#include <iostream>
#include <thread>

Server::Server() : m_database(std::make_shared<SqliteDataBase>()), m_handlerFactory(std::make_shared<RequestHandlerFactory>(m_database)),
    m_communicator(m_handlerFactory)
{

}

void Server::run()
{
    std::thread communicatorThread(&Communicator::startHandleRequests, &m_communicator);

    std::string userInput = "";

    while (true)
    {
        std::cin >> userInput;

        if (userInput == EXIT_SERVER)
        {
            std::cout << "Server is shutting down as requested..." << std::endl;
            this->m_communicator.stopCommunicating();
            break;
        }

        system("cls");
        std::cout << "Server listening on port " << SERVER_PORT << std::endl << std::endl;

    }

    communicatorThread.join();
}

#include "Server.h"
#include "Constants.h"

#include <iostream>
#include <thread>

void Server::run()
{
    m_database->open();
    // Launching communicator thread which handles new clients' requests
    std::thread communicatorThread(&Communicator::startHandleRequests, &m_communicator);
    communicatorThread.detach();

    std::string userInput = "";

    while (true)
    {
        std::cin >> userInput;

        if (userInput == EXIT_SERVER)
        {
            std::cout << "Server is shutting down as requested..." << std::endl;
            break;
        }

        system("cls");
        std::cout << "Server listening on port " << SERVER_PORT << std::endl << std::endl;

    }
    m_database->close();
}

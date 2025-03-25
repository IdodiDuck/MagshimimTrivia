#pragma comment (lib, "ws2_32.lib")

#include "WSAInitializer.h"
#include "Server.h"
#include "Constants.h"

#include <iostream>
#include <exception>

int main()
{

	try
	{
		WSAInitializer wsaInit;
		Server myServer;

		myServer.run(SERVER_PORT);
	}

	catch (const std::exception& e)
	{
		std::cout << "Error occured: " << e.what() << std::endl;
	}

	system("PAUSE");
	return 0;
}

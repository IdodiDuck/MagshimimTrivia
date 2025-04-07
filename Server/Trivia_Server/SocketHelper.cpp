#include "SocketHelper.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

using std::string;

std::vector<unsigned char> SocketHelper::getData(const SOCKET sc, const int bytesNum)
{
	const int DEFAULT_FLAG = 0;

	std::vector<unsigned char> data(bytesNum);

	int res = recv(sc, reinterpret_cast<char*>(data.data()), bytesNum, DEFAULT_FLAG);
	if (res == INVALID_SOCKET)
	{
		std::string s = "Error while receiving from socket: ";
		s += std::to_string(sc);
		throw std::exception(s.c_str());
	}

	return data;

}

// recieve data from socket according byteSize
// returns the data as int
int SocketHelper::getIntPartFromSocket(const SOCKET sc, const int bytesNum)
{
	std::string valueAsString = (getPartFromSocket(sc, bytesNum, 0).c_str());
	char* value = valueAsString.data();
	unsigned char* numericValues = (unsigned char*)(value);

	int currentByteDecimalValue = 0;

	for (int currentByte = 0; currentByte < bytesNum; currentByte++)
	{
		currentByteDecimalValue += (int)(numericValues[currentByte]);
	}

	return currentByteDecimalValue;
}

// recieve data from socket according byteSize
// returns the data as string
string SocketHelper::getStringPartFromSocket(const SOCKET sc, const int bytesNum)
{
	return getPartFromSocket(sc, bytesNum, 0);
}


// recieve data from socket according byteSize
// this is private function
std::string SocketHelper::getPartFromSocket(const SOCKET sc, const int bytesNum)
{
	return getPartFromSocket(sc, bytesNum, 0);
}

// send data to socket
// this is private function
void SocketHelper::sendData(const SOCKET sc, const std::string& message)
{
	const char* data = message.c_str();

	if (send(sc, data, message.size(), 0) == INVALID_SOCKET)
	{
		throw std::exception("Error while sending message to client");
	}
}

std::string SocketHelper::getPartFromSocket(const SOCKET sc, const int bytesNum, const int flags)
{
	if (bytesNum == 0)
	{
		return "";
	}

	char* data = new char[bytesNum + 1];
	int res = recv(sc, data, bytesNum, flags);
	if (res == INVALID_SOCKET)
	{
		std::string s = "Error while recieving from socket: ";
		s += std::to_string(sc);
		throw std::exception(s.c_str());
	}

	data[bytesNum] = 0;
	std::string received(data);
	delete[] data;
	return received;
}
#include "SocketHelper.h"

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

int SocketHelper::getRequestCode(const SOCKET sc)
{
	const int REQUEST_CODE_BYTES = 1;

	std::vector<unsigned char> header = getData(sc, REQUEST_CODE_BYTES);
	return header[0]; // The received vector is supposed to have only the first byte (which is the code of request)

}

int SocketHelper::getRequestLength(const SOCKET sc)
{
	const int REQUEST_LENGTH_BYTES = 4;

	std::vector<unsigned char> header = getData(sc, REQUEST_LENGTH_BYTES);

	int messageCode = 0;
	for (int currentByte = 0; currentByte < REQUEST_LENGTH_BYTES; currentByte++)
	{
		messageCode |= (header[currentByte] << (8 * (REQUEST_LENGTH_BYTES - currentByte - 1)));
	}

	int requestLength = header[REQUEST_LENGTH_BYTES - 1];
	return requestLength;
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
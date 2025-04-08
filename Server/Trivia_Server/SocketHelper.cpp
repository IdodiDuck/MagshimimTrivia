#include "SocketHelper.h"

using std::string;

std::optional<std::vector<unsigned char>> SocketHelper::getData(const SOCKET sc, const int bytesNum)
{
	const int DEFAULT_FLAG = 0;

	std::vector<unsigned char> data(bytesNum);

	int res = recv(sc, reinterpret_cast<char*>(data.data()), bytesNum, DEFAULT_FLAG);

	if (res == INVALID_SOCKET)
	{
		return std::nullopt;
	}

	return data;

}

std::optional<int> SocketHelper::getRequestCode(const SOCKET sc)
{
	const int REQUEST_CODE_BYTES = 1;

	auto headerOpt = getData(sc, REQUEST_CODE_BYTES);  // Get data from socket

	if (!headerOpt)  // If no data received or error occurred
	{
		return std::nullopt;
	}

	return headerOpt.value()[0]; // Return the first byte as the request code

}

std::optional<int> SocketHelper::getRequestLength(const SOCKET sc)
{
	const int REQUEST_LENGTH_BYTES = 4, BYTE_SIZE = 8;

	auto headerOpt = getData(sc, REQUEST_LENGTH_BYTES);

	if (!headerOpt)  // If no data received or error occurred
	{
		return std::nullopt;
	}

	const std::vector<unsigned char>& header = headerOpt.value();

	int messageCode = 0;

	// Shifting and performing OR logical operation on the bytes to assemble the message code
	for (int currentByte = 0; currentByte < REQUEST_LENGTH_BYTES; currentByte++)
	{
		messageCode |= (header[currentByte] << (BYTE_SIZE * (REQUEST_LENGTH_BYTES - currentByte - 1)));
	}

	// Return the last byte as the request length
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
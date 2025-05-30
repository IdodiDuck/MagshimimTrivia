#include "SocketHelper.h"

#include <iostream>

std::optional<std::vector<unsigned char>> SocketHelper::getData(const SOCKET sc, const int bytesNum)
{
	const int DEFAULT_FLAG = 0, CLIENT_CLOSED_SOCKET = 0;

	std::vector<unsigned char> data(bytesNum);

	int res = recv(sc, reinterpret_cast<char*>(data.data()), bytesNum, DEFAULT_FLAG);

	if (res == SOCKET_ERROR)
	{
		std::cerr << "[ERROR]: Unexpected Error with Client's Socket: WSA Last Error: " << WSAGetLastError() << std::endl;
		return std::nullopt;
	}

	else if (res == CLIENT_CLOSED_SOCKET)
	{
		std::cerr << "[ERROR]: Client has closed his socket!" << std::endl;
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

	// If we didn't received enough bytes for reading
	if (header.size() < REQUEST_LENGTH_BYTES)
	{
		return std::nullopt;
	}

	int messageCode = 0;

	// Shifting and performing OR logical operation on the bytes to assemble the message code
	for (int currentByte = 0; currentByte < REQUEST_LENGTH_BYTES; currentByte++)
	{
		messageCode |= (header[currentByte] << (BYTE_SIZE * (REQUEST_LENGTH_BYTES - currentByte - 1)));
	}

	int requestLength = header[REQUEST_LENGTH_BYTES - 1];  // Might want to check if this logic is correct
	return requestLength;
}

void SocketHelper::sendData(const SOCKET sc, const std::vector<unsigned char>& data)
{
	int dataSize = static_cast<int>(data.size());
	if (send(sc, reinterpret_cast<const char*>(data.data()), dataSize, 0) == INVALID_SOCKET)
	{
		throw std::exception("Error while sending message to client");
	}
}
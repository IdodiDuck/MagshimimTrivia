#pragma once

#include <vector>
#include <string>
#include <optional>
#include <WinSock2.h>

class SocketHelper
{
public:

	static std::vector<unsigned char> getData(const SOCKET sc, const int bytesNum);
	static int getRequestCode(const SOCKET sc);
	static int getRequestLength(const SOCKET sc);
	static void sendData(const SOCKET sc, const std::string& message);
};


#ifdef _DEBUG // vs add this define in debug mode
#include <stdio.h>
// Q: why do we need traces ?
// A: traces are a nice and easy way to detect bugs without even debugging
// or to understand what happened in case we miss the bug in the first time
#define TRACE(msg, ...) printf(msg "\n", __VA_ARGS__);
// for convenient reasons we did the traces in stdout
// at general we would do this in the error stream like that
// #define TRACE(msg, ...) fprintf(stderr, msg "\n", __VA_ARGS__);

#else // we want nothing to be printed in release version
#define TRACE(msg, ...) printf(msg "\n", __VA_ARGS__);
#define TRACE(msg, ...) // do nothing
#endif
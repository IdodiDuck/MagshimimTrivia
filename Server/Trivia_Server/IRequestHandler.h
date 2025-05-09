#pragma once

#include <memory>
#include <ctime>
#include <vector>

class IRequestHandler;

// Structures (Related to RequestHandler Interface) -
typedef struct RequestInfo
{
    int requestID;
    std::time_t receivalTime;
    std::vector<unsigned char> buffer;

} RequestInfo;

typedef struct RequestResult
{
    std::vector<unsigned char> response;
    std::unique_ptr<IRequestHandler> newHandler;

} RequestResult;

class IRequestHandler
{

public:
	virtual ~IRequestHandler() = default;

	virtual bool isRequestRelevant(const RequestInfo& info) = 0;
	virtual RequestResult handleRequest(const RequestInfo& info) = 0;
};

#pragma once

#include "IRequestHandler.h"

class LoginRequestHandler: public IRequestHandler
{

public:

	virtual ~LoginRequestHandler();

	bool isRequestRelevant(const RequestInfo& info) override;
	RequestResult handleRequest(RequestInfo& info) override;
};

#pragma once

#include "IRequestHandler.h"

class MenuRequestHandler : public IRequestHandler
{

public:

	MenuRequestHandler();
	virtual ~MenuRequestHandler();

	bool isRequestRelevant(const RequestInfo& info) override;
	RequestResult handleRequest(RequestInfo& info) override;
};

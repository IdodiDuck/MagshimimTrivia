#pragma once

#include "IRequestHandler.h"
#include "RequestHandlerFactory.h"

class LoginRequestHandler: public IRequestHandler
{

public:

	LoginRequestHandler(RequestHandlerFactory& handlerFactory);
	virtual ~LoginRequestHandler();

	bool isRequestRelevant(const RequestInfo& info) override;
	RequestResult handleRequest(const RequestInfo& info) override;

private:
	RequestHandlerFactory& m_handlerFactory;

	RequestResult login(const RequestInfo& request);
	RequestResult signup(const RequestInfo& request);

};

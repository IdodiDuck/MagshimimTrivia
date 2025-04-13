#pragma once

#include "IRequestHandler.h"
#include "RequestHandlerFactory.h"

class LoginRequestHandler: public IRequestHandler
{

public:

	LoginRequestHandler(RequestHandlerFactory& handlerFactory);
	virtual ~LoginRequestHandler();

	bool isRequestRelevant(const RequestInfo& info) override;
	RequestResult handleRequest(RequestInfo& info) override;

private:
	RequestHandlerFactory& m_handlerFactory;

	RequestResult login(RequestInfo request);
	RequestResult signup(RequestInfo request);

};

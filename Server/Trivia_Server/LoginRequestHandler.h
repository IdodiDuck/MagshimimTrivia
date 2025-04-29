#pragma once

#include "IRequestHandler.h"

class RequestHandlerFactory;

class LoginRequestHandler: public IRequestHandler
{

public:
	// C'tor & D'tor - 
	LoginRequestHandler(std::weak_ptr<RequestHandlerFactory> handlerFactory);
	virtual ~LoginRequestHandler();

	// Virtuals - 
	bool isRequestRelevant(const RequestInfo& info) override;
	RequestResult handleRequest(const RequestInfo& info) override;

private:
	// Attributes - 
	std::weak_ptr<RequestHandlerFactory> m_handlerFactory;

	// Login Operation Methods - 
	RequestResult login(const RequestInfo& request);
	RequestResult signup(const RequestInfo& request);

	// Support Methods - 
	std::shared_ptr<RequestHandlerFactory> getFactorySafely();
};

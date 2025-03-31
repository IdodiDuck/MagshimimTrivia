#include "LoginRequestHandler.h"

#include "Constants.h"

LoginRequestHandler::~LoginRequestHandler()
{
    // TODO later
}

bool LoginRequestHandler::isRequestRelevant(const RequestInfo& info)
{
    return ((info.requestID == SIGNUP_REQUEST_CODE) || (info.requestID == LOGIN_REQUEST_CODE));
}

RequestResult LoginRequestHandler::handleRequest(RequestInfo& info)
{
    return RequestResult();
}

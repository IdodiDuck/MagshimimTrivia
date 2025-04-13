#include "MenuRequestHandler.h"

MenuRequestHandler::MenuRequestHandler()
{

}

MenuRequestHandler::~MenuRequestHandler()
{

}

bool MenuRequestHandler::isRequestRelevant(const RequestInfo& info)
{
    return false; // Default return for now, will be modified
}

RequestResult MenuRequestHandler::handleRequest(RequestInfo& info)
{
    return RequestResult(); // Default return for now, will be modified
}

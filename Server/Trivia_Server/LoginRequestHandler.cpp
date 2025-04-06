#include "LoginRequestHandler.h"

#include "JsonRequestPacketDeserializer.h"
#include "JsonResponsePacketSerializer.h"

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
    if (info.requestID == LOGIN_REQUEST_CODE)
    {
        auto loginRequest = JsonRequestPacketDeserializer::deserializeLoginRequest(info.buffer);
        
        // Valid login request
        if (loginRequest.has_value())
        {
            LoginRequest request = loginRequest.value();
            // Database Login Validation here (V1.0.3)
            
            LoginResponse response;
            response.status = SUCCESS;
            RequestResult result;
            result.response = JsonResponsePacketSerializer::serializeResponse(response);
            result.newHandler = nullptr; // Modify later and change to next given handler

            return result;
        }
        // TODO in-case of failure
    }

    else if (info.requestID == SIGNUP_REQUEST_CODE)
    {
        auto singupRequest = JsonRequestPacketDeserializer::deserializeSignupRequest(info.buffer);

        // Valid login request
        if (singupRequest.has_value())
        {
            SignupRequest request = singupRequest.value();
            // Database Login Validation here (V1.0.3)

            SignupResponse response;
            response.status = SUCCESS;
            RequestResult result;
            result.response = JsonResponsePacketSerializer::serializeResponse(response);
            result.newHandler = nullptr; // Modify later and change to next given handler

            return result;
        }
        // TODO in-case of failure
    }
}

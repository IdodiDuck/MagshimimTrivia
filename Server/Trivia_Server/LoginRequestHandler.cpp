#include "LoginRequestHandler.h"

#include "JsonRequestPacketDeserializer.h"
#include "JsonResponsePacketSerializer.h"

#include "Constants.h"

#include <iostream>

LoginRequestHandler::~LoginRequestHandler()
{
    
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
            result.newHandler.reset(); // Modify later and change to next given handler

            return result;
        }
        
        // Constructing failure message in-case it didn't work
        RequestResult result;
        ErrorResponse response;
        response.message = "Invalid login request format.";
        std::vector<unsigned char> resultBuffer = JsonResponsePacketSerializer::serializeResponse(response);
        result.response = resultBuffer;
        result.newHandler.reset();

        return result;
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
            result.newHandler.reset(); // Modify later and change to next given handler

            return result;
        }
        
        // Constructing failure message in-case it didn't work
        RequestResult result;
        ErrorResponse response;
        response.message = "Invalid sign-up request format.";
        std::vector<unsigned char> resultBuffer = JsonResponsePacketSerializer::serializeResponse(response);
        result.response = resultBuffer;
        result.newHandler.reset();

        return result;
    }
}

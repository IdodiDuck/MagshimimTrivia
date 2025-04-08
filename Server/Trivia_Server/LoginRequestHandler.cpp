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
    return ((info.requestID == static_cast<unsigned int>(RequestCode::SIGNUP_REQUEST)) || (info.requestID == static_cast<unsigned int>(RequestCode::LOGIN_REQUEST)));
}

RequestResult LoginRequestHandler::handleRequest(RequestInfo& info)
{
    RequestResult result;

    if (!(isRequestRelevant(info)))
    {
        ErrorResponse errorResponse;
        errorResponse.message = "Error: Invalid request type.";
        result.response = JsonResponsePacketSerializer::serializeResponse(errorResponse);
        result.newHandler.reset();

        return result; 
    }

    switch (static_cast<RequestCode>(info.requestID))
    {
        case RequestCode::LOGIN_REQUEST:
        {
            auto loginRequest = JsonRequestPacketDeserializer::deserializeLoginRequest(info.buffer);

            // Valid login request
            if (loginRequest.has_value())
            {
                LoginRequest request = loginRequest.value();
                // Database Login Validation here (V1.0.3)

                LoginResponse response;
                response.status = SUCCESS;
                result.response = JsonResponsePacketSerializer::serializeResponse(response);
                result.newHandler.reset(); // Modify later and change to next given handler

                return result;
            }

            // Constructing failure message in-case it didn't work
            ErrorResponse errorResponse;
            errorResponse.message = "Invalid login request format.";
            result.response = JsonResponsePacketSerializer::serializeResponse(errorResponse);
            result.newHandler.reset();

            return result;
        }

        case RequestCode::SIGNUP_REQUEST:
        {
            auto signupRequest = JsonRequestPacketDeserializer::deserializeSignupRequest(info.buffer);

            // Valid signup request
            if (signupRequest.has_value())
            {
                SignupRequest request = signupRequest.value();
                // Database Signup Validation here (V1.0.3)

                SignupResponse response;
                response.status = SUCCESS;
                result.response = JsonResponsePacketSerializer::serializeResponse(response);
                result.newHandler.reset(); // Modify later and change to next given handler

                return result;
            }

            // Constructing failure message in-case it didn't work
            ErrorResponse errorResponse;
            errorResponse.message = "Invalid sign-up request format.";
            result.response = JsonResponsePacketSerializer::serializeResponse(errorResponse);
            result.newHandler.reset();

            return result;
        }

        default:
        {
            // Handle unknown request type (optional)
            ErrorResponse errorResponse;
            errorResponse.message = "Error: Unknown request type.";
            result.response = JsonResponsePacketSerializer::serializeResponse(errorResponse);
            result.newHandler.reset();

            return result;
        }
    }
}

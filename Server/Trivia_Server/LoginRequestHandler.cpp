#include "LoginRequestHandler.h"

#include "JsonRequestPacketDeserializer.h"
#include "JsonResponsePacketSerializer.h"

#include "Constants.h"

#include "LoginManager.h"
#include "SqliteDataBase.h"

#include <iostream>

LoginRequestHandler::LoginRequestHandler(RequestHandlerFactory& handlerFactory) : m_handlerFactory(handlerFactory)
{

}

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
    auto SQLitedatabase = std::make_shared<SqliteDataBase>(); // Creating the data base and opening it in it's C'tor

    std::shared_ptr<IDatabase> database = SQLitedatabase; // Converting the pointer to IDatabase so it can work with LoginManager
    std::weak_ptr<IDatabase> weakDatabase = database;

    LoginManager loginManager(weakDatabase);

    switch (static_cast<RequestCode>(info.requestID))
    {
        case RequestCode::LOGIN_REQUEST:
        {
            auto loginRequest = JsonRequestPacketDeserializer::deserializeLoginRequest(info.buffer);

            // Valid login request
            if (loginRequest.has_value())
            {
                LoginRequest request = loginRequest.value(); // Accessing the login result from the returned optional

                LoginStatus loginStatus = loginManager.login(request.username, request.password);

                LoginResponse response;
                response.status = (loginStatus == LoginStatus::SUCCESS ? SUCCESS : FAILURE);
                result.response = JsonResponsePacketSerializer::serializeResponse(response);
                result.newHandler.reset(); // Change to next handler if needed

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
                SignupRequest request = signupRequest.value(); // Accessing the signup result from the returned optional

                // Validate signup through LoginManager
                SignUpStatus signUpStatus = loginManager.signUp(request.username, request.password, request.email);

                SignupResponse response;
                response.status = (signUpStatus == SignUpStatus::SUCCESS ? SUCCESS : FAILURE);
                result.response = JsonResponsePacketSerializer::serializeResponse(response);
                result.newHandler.reset(); // Modify later and change to next given handler

                return result;
            }

            // If the signup request format is invalid
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
            errorResponse.message = "Unknown request type.";
            result.response = JsonResponsePacketSerializer::serializeResponse(errorResponse);
            result.newHandler.reset();

            return result;
        }
    }
}

RequestResult LoginRequestHandler::login(RequestInfo request)
{
    LoginRequest loginRequest = JsonRequestPacketDeserializer::deserializeLoginRequest(request.buffer).value();

    if (m_handlerFactory.getLoginManager().login(loginRequest.username, loginRequest.password) == LoginStatus::SUCCESS)
    {
        return 
        { 
            JsonResponsePacketSerializer::serializeResponse(LoginResponse()), 
            (std::unique_ptr<IRequestHandler>)std::make_unique<LoginRequestHandler>(m_handlerFactory)
        };
    }
    return 
    { 
        JsonResponsePacketSerializer::serializeResponse(ErrorResponse()), 
        (std::unique_ptr<IRequestHandler>)std::make_unique<LoginRequestHandler>(m_handlerFactory)
     };

}

RequestResult LoginRequestHandler::signup(RequestInfo request)
{
    SignupRequest signupRequest = JsonRequestPacketDeserializer::deserializeSignupRequest(request.buffer).value();

    if (m_handlerFactory.getLoginManager().signUp(signupRequest.username, signupRequest.password, signupRequest.email) == SignUpStatus::SUCCESS)
    {
        return 
        { 
            JsonResponsePacketSerializer::serializeResponse(SignupResponse()), 
            (std::unique_ptr<IRequestHandler>)std::make_unique<LoginRequestHandler>(m_handlerFactory) 
        };
    }
    return 
    { 
        JsonResponsePacketSerializer::serializeResponse(ErrorResponse()),
        (std::unique_ptr<IRequestHandler>)std::make_unique<LoginRequestHandler>(m_handlerFactory) 
    };
}

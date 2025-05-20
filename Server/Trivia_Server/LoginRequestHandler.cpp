#include "LoginRequestHandler.h"

#include "JsonRequestPacketDeserializer.h"
#include "JsonResponsePacketSerializer.h"

#include "RequestHandlerFactory.h"
#include "ManagerException.h"

#include <iostream>

LoginRequestHandler::LoginRequestHandler(std::weak_ptr<RequestHandlerFactory> handlerFactory): m_handlerFactory(handlerFactory)
{

}

LoginRequestHandler::~LoginRequestHandler()
{
    
}

bool LoginRequestHandler::isRequestRelevant(const RequestInfo& info)
{
    return ((info.requestID == static_cast<unsigned int>(RequestCode::SIGNUP_REQUEST)) || (info.requestID == static_cast<unsigned int>(RequestCode::LOGIN_REQUEST)));
}

RequestResult LoginRequestHandler::handleRequest(const RequestInfo& info)
{
    try
    {
        switch (static_cast<RequestCode>(info.requestID))
        {
            case RequestCode::LOGIN_REQUEST:
                return login(info);

            case RequestCode::SIGNUP_REQUEST:
                return signup(info);

            default:
                throw ManagerException("Error: Unknown Type of Request was sent!");
        }
    }
    
    catch (const ManagerException& e)
    {
        ErrorResponse errorResponse = { };
        errorResponse.message = e.what();
        RequestResult requestResult;
        requestResult.response = JsonResponsePacketSerializer::serializeResponse(errorResponse);
        requestResult.newHandler = this->getFactorySafely()->createLoginRequestHandler();
        return requestResult;
    }

    catch (const ServerException& e)
    {
        ErrorResponse errorResponse = { };
        errorResponse.message = "Server Error: " + std::string(e.what());
        RequestResult requestResult;
        requestResult.response = JsonResponsePacketSerializer::serializeResponse(errorResponse);
        requestResult.newHandler = this->getFactorySafely()->createLoginRequestHandler();
        return requestResult;
    }
}

void LoginRequestHandler::handleDisconnection()
{
    std::cerr << "[LOGIN REQUEST HANDLER]: User decided to disconnect..." << std::endl;
}

RequestResult LoginRequestHandler::login(const RequestInfo& request)
{
    LoginRequest loginRequest = JsonRequestPacketDeserializer::deserializeLoginRequest(request.buffer).value();
    LoginStatus status = getFactorySafely()->getLoginManager().login(loginRequest.username, loginRequest.password);
    LoginResponse response {};

    switch (status)
    {
        case LoginStatus::SUCCESS:
            response.status = SUCCESS;
            return
            {
                JsonResponsePacketSerializer::serializeResponse(response),
                getFactorySafely()->createMenuRequestHandler(LoggedUser(loginRequest.username))
            };

        case LoginStatus::USER_ALREADY_LOGGED_IN:
            throw ManagerException("Error: User is already logged in!");

        case LoginStatus::USER_NOT_EXISTS:
            throw ManagerException("Error: User does not exist!");

        case LoginStatus::DISMATCHING_PASSWORD:
            throw ManagerException("Error: Wrong password was entered!");

        default:
            throw ManagerException("Error: Login had failed! Please try again!");
    }
}

RequestResult LoginRequestHandler::signup(const RequestInfo& request)
{
    SignupRequest signupRequest = JsonRequestPacketDeserializer::deserializeSignupRequest(request.buffer).value();
    SignUpStatus status = getFactorySafely()->getLoginManager().signUp(signupRequest.username, signupRequest.password, signupRequest.email);

    SignupResponse response {};

    switch (status)
    {
        case SignUpStatus::SUCCESS:
            response.status = SUCCESS;

            return
            {
                JsonResponsePacketSerializer::serializeResponse(response),
                getFactorySafely()->createMenuRequestHandler(LoggedUser(signupRequest.username))
            };

        case SignUpStatus::USER_ALREADY_EXISTS:
            throw ManagerException("Error: Can't sign up, User already exists!");

        default:
            throw ManagerException("Error: Signup had failed! Please try again!");
    }
}

std::shared_ptr<RequestHandlerFactory> LoginRequestHandler::getFactorySafely()
{
    if (auto factory = m_handlerFactory.lock())
    {
        return factory;
    }

    throw std::runtime_error("LoginRequestHandler: [ERROR]: RequestHandlerFactory is no longer available");
}

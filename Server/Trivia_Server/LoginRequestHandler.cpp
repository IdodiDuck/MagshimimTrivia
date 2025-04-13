#include "LoginRequestHandler.h"

#include "JsonRequestPacketDeserializer.h"
#include "JsonResponsePacketSerializer.h"

LoginRequestHandler::LoginRequestHandler(RequestHandlerFactory& handlerFactory): m_handlerFactory(handlerFactory)
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
    switch (static_cast<RequestCode>(info.requestID))
    {
        case RequestCode::LOGIN_REQUEST:
            return login(info);

        case RequestCode::SIGNUP_REQUEST:
            return signup(info);

        default:
            ErrorResponse errorResponse;
            errorResponse.message = "Unknown request type.";
            return 
            {
                JsonResponsePacketSerializer::serializeResponse(errorResponse),
                nullptr
            };
    }
}

RequestResult LoginRequestHandler::login(const RequestInfo& request)
{
    LoginRequest loginRequest = JsonRequestPacketDeserializer::deserializeLoginRequest(request.buffer).value();

    if (m_handlerFactory.getLoginManager().login(loginRequest.username, loginRequest.password) == LoginStatus::SUCCESS)
    {
        return
        {
            JsonResponsePacketSerializer::serializeResponse(LoginResponse()),
            (std::unique_ptr<IRequestHandler>)(this->m_handlerFactory.createMenuRequestHandler())
        };
    }

    return 
    { 
        JsonResponsePacketSerializer::serializeResponse(ErrorResponse()), 
        (std::unique_ptr<IRequestHandler>)std::make_unique<LoginRequestHandler>(m_handlerFactory)
     };

}

RequestResult LoginRequestHandler::signup(const RequestInfo& request)
{
    SignupRequest signupRequest = JsonRequestPacketDeserializer::deserializeSignupRequest(request.buffer).value();

    if (m_handlerFactory.getLoginManager().signUp(signupRequest.username, signupRequest.password, signupRequest.email) == SignUpStatus::SUCCESS)
    {
        return 
        { 
            JsonResponsePacketSerializer::serializeResponse(SignupResponse()), 
            (std::unique_ptr<IRequestHandler>)(this->m_handlerFactory.createMenuRequestHandler())
        };
    }

    return 
    { 
        JsonResponsePacketSerializer::serializeResponse(ErrorResponse()),
        (std::unique_ptr<IRequestHandler>)std::make_unique<LoginRequestHandler>(m_handlerFactory) 
    };
}

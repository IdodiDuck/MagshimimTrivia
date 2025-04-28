#include "MenuRequestHandler.h"

#include "Constants.h"
#include "JsonResponsePacketSerializer.h"

#include <memory>

MenuRequestHandler::MenuRequestHandler(const LoggedUser& user, RequestHandlerFactory& handlerFactory): m_user(user), m_handlerFactory(handlerFactory)
{

}

MenuRequestHandler::~MenuRequestHandler()
{

}

bool MenuRequestHandler::isRequestRelevant(const RequestInfo& info)
{
    return (info.requestID == static_cast<unsigned int>(RequestCode::SIGNOUT_REQUEST)) ||
           (info.requestID == static_cast<unsigned int>(RequestCode::ROOMS_REQUEST)) ||
           (info.requestID == static_cast<unsigned int>(RequestCode::PLAYERS_IN_ROOM_REQUEST)) ||
           (info.requestID == static_cast<unsigned int>(RequestCode::PERSONAL_STATS_REQUEST)) ||
           (info.requestID == static_cast<unsigned int>(RequestCode::HIGH_SCORE_REQUEST)) ||
           (info.requestID == static_cast<unsigned int>(RequestCode::JOIN_ROOM_REQUEST)) ||
           (info.requestID == static_cast<unsigned int>(RequestCode::CREATE_ROOM_REQUEST));
}

RequestResult MenuRequestHandler::handleRequest(const RequestInfo& info)
{
    switch (static_cast<RequestCode>(info.requestID))
    {
        case RequestCode::SIGNOUT_REQUEST:
            return signout(info);

        case RequestCode::ROOMS_REQUEST:
            return getRooms(info);

        case RequestCode::PLAYERS_IN_ROOM_REQUEST:
            return getPlayersInRoom(info);

        case RequestCode::PERSONAL_STATS_REQUEST:
            return getPersonalStats(info);

        case RequestCode::HIGH_SCORE_REQUEST:
            return getHighScore(info);

        case RequestCode::JOIN_ROOM_REQUEST:
            return joinRoom(info);

        case RequestCode::CREATE_ROOM_REQUEST:
            return createRoom(info);

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

RequestResult MenuRequestHandler::signout(const RequestInfo& info)
{
    LogoutResponse response;
    response.status = SUCCESS;

    return
    {
        JsonResponsePacketSerializer::serializeResponse(response),
        std::make_unique<IRequestHandler>(this->m_handlerFactory.createLoginRequestHandler())
    };

}

RequestResult MenuRequestHandler::getRooms(const RequestInfo& info)
{
    return RequestResult();
}

RequestResult MenuRequestHandler::getPlayersInRoom(const RequestInfo& info)
{
    return RequestResult();
}

RequestResult MenuRequestHandler::getPersonalStats(const RequestInfo& info)
{
    return RequestResult();
}

RequestResult MenuRequestHandler::getHighScore(const RequestInfo& info)
{
    return RequestResult();
}

RequestResult MenuRequestHandler::joinRoom(const RequestInfo& info)
{
    return RequestResult();
}

RequestResult MenuRequestHandler::createRoom(const RequestInfo& info)
{
    return RequestResult();
}

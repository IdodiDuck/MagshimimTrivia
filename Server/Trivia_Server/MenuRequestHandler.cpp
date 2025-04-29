#include "MenuRequestHandler.h"

#include "Constants.h"
#include "RequestHandlerFactory.h"

#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"

MenuRequestHandler::MenuRequestHandler(std::weak_ptr<RequestHandlerFactory> handlerFactory, const LoggedUser& user): m_user(user), m_handlerFactory(handlerFactory)
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
        getFactorySafely()->createLoginRequestHandler()
    };
}

RequestResult MenuRequestHandler::getRooms(const RequestInfo& info)
{
    GetRoomsResponse response;
    response.status = SUCCESS;
    
    return
    {
        JsonResponsePacketSerializer::serializeResponse(response),
        nullptr // To do - appoint to a new handler when added
    };
}

RequestResult MenuRequestHandler::getPlayersInRoom(const RequestInfo& info)
{
    GetPlayersInRoomRequest request = JsonRequestPacketDeserializer::deserializeGetPlayersRequest(info.buffer).value();
    Room currentRoom = getFactorySafely()->getRoomManager().getRoom(request.roomId).value();
    GetPlayersInRoomResponse response;

    for (const auto& username : currentRoom.getAllUsers())
    {
        response.players.push_back(username);
    }

    return
    {
        JsonResponsePacketSerializer::serializeResponse(response),
        nullptr
    };

}

RequestResult MenuRequestHandler::getPersonalStats(const RequestInfo& info)
{
    getPersonalStatsResponse response;
    response.status = SUCCESS;
    response.statistics = getFactorySafely()->getStatisticsManager().getUserStatistics(m_user.getUserName());

    return
    {
        JsonResponsePacketSerializer::serializeResponse(response),
        nullptr
    };
}

RequestResult MenuRequestHandler::getHighScore(const RequestInfo& info)
{
    getHighScoreResponse response;
    response.status = SUCCESS;
    response.statistics = getFactorySafely()->getStatisticsManager().getUserStatistics(m_user.getUserName());

    return
    {
        JsonResponsePacketSerializer::serializeResponse(response),
        nullptr
    };
}

RequestResult MenuRequestHandler::joinRoom(const RequestInfo& info)
{
    const unsigned int INVALID_ROOM_ID = 0;

    JoinRoomRequest request = JsonRequestPacketDeserializer::deserializeJoinRoomRequest(info.buffer).value();
    JoinRoomResponse response;

    response.status = (request.roomId != INVALID_ROOM_ID) ? SUCCESS : FAILURE;

    return
    {
        JsonResponsePacketSerializer::serializeResponse(response),
        nullptr // To do - appoint to a new handler when added
    };
}

RequestResult MenuRequestHandler::createRoom(const RequestInfo& info)
{
    CreateRoomRequest request = JsonRequestPacketDeserializer::deserializeCreateRoomRequest(info.buffer).value();
    CreateRoomResponse response;

    response.status = (request.roomName != "") ? SUCCESS : FAILURE;

    return
    {
        JsonResponsePacketSerializer::serializeResponse(response),
        nullptr // To do - appoint to a new handler when added
    };
}

std::shared_ptr<RequestHandlerFactory> MenuRequestHandler::getFactorySafely()
{
    if (auto factory = m_handlerFactory.lock())
    {
        return factory;
    }

    throw std::runtime_error("MenuRequestHandler: [ERROR]: RequestHandlerFactory is no longer available");
}

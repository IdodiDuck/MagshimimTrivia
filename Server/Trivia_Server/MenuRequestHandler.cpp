#include "MenuRequestHandler.h"

#include "Constants.h"
#include "RequestHandlerFactory.h"

#include "JsonResponsePacketSerializer.h"
#include "JsonRequestPacketDeserializer.h"

#include "ManagerException.h"

std::atomic<unsigned int> MenuRequestHandler::m_currentRoomId(1);

MenuRequestHandler::MenuRequestHandler(std::weak_ptr<RequestHandlerFactory> handlerFactory, const LoggedUser& user): m_user(user), m_handlerFactory(handlerFactory)
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
    try
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
                throw ManagerException("Error: Unknown Type of Request was sent!");
        }
    }

    catch (const ManagerException& e)
    {
        ErrorResponse errorResponse = { };
        errorResponse.message = e.what();
        RequestResult requestResult;
        requestResult.response = JsonResponsePacketSerializer::serializeResponse(errorResponse);
        requestResult.newHandler = this->getFactorySafely()->createMenuRequestHandler(this->m_user);
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

void MenuRequestHandler::handleDisconnection()
{
    this->getFactorySafely()->getLoginManager().logOut(this->m_user.getUserName());
}

RequestResult MenuRequestHandler::signout(const RequestInfo& info)
{
    LogoutResponse response { SUCCESS };

    getFactorySafely()->getLoginManager().logOut(this->m_user.getUserName());

    return
    {
        JsonResponsePacketSerializer::serializeResponse(response),
        getFactorySafely()->createLoginRequestHandler()
    };
}

RequestResult MenuRequestHandler::getRooms(const RequestInfo& info)
{
    GetRoomsResponse response { SUCCESS, getFactorySafely()->getRoomManager().getRooms() };
    
    return
    {
        JsonResponsePacketSerializer::serializeResponse(response),
        std::make_unique<MenuRequestHandler>(*this)
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
        std::make_unique<MenuRequestHandler>(*this)
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
        std::make_unique<MenuRequestHandler>(*this)
    };
}

RequestResult MenuRequestHandler::getHighScore(const RequestInfo& info)
{
    getHighScoreResponse response;
    response.status = SUCCESS;
    response.statistics = getFactorySafely()->getStatisticsManager().getHighScore();

    return
    {
        JsonResponsePacketSerializer::serializeResponse(response),
        std::make_unique<MenuRequestHandler>(*this)
    };
}

RequestResult MenuRequestHandler::joinRoom(const RequestInfo& info)
{
    JoinRoomRequest request = JsonRequestPacketDeserializer::deserializeJoinRoomRequest(info.buffer).value();
    JoinRoomResponse response = {};

    auto roomOpt = getFactorySafely()->getRoomManager().getRoomReference(request.roomId);

    if (!roomOpt.has_value())
    {
        throw ManagerException("Error: Room doesn't exist!");
    }

    Room& room = roomOpt.value();

    switch (room.getRoomData().status)
    {
        case RoomStatus::CLOSED:
            throw ManagerException("Error: Room is closed!");

        case RoomStatus::GAME_STARTED:
            throw ManagerException("Error: Room is in the middle of an active game!");

        default:
            // Room is at active status -> User can possibly join
            break;
    }

    if (room.isRoomFull())
    {
        throw ManagerException("Error: Room is at full capacity!");
    }

    room.addUser(this->m_user);
    response.status = SUCCESS;

    return
    {
        JsonResponsePacketSerializer::serializeResponse(response),
        getFactorySafely()->createRoomMemberRequestHandler(this->m_user, request.roomId)
    };
}


RequestResult MenuRequestHandler::createRoom(const RequestInfo& info)
{
    CreateRoomRequest request = JsonRequestPacketDeserializer::deserializeCreateRoomRequest(info.buffer).value();
    CreateRoomResponse response {};

    RoomData addedRoomData = { };
    addedRoomData.maxPlayers = request.maxUsers;
    addedRoomData.name = request.roomName;
    addedRoomData.numOfQuestionsInGame = request.questionCount;
    addedRoomData.timePerQuestion = request.answerTimeout;
    addedRoomData.status = RoomStatus::OPENED;
    addedRoomData.id = generateNewRoomId();

    this->getFactorySafely()->getRoomManager().createRoom(m_user, addedRoomData);

    response.status = (request.roomName != "") ? SUCCESS : FAILURE;

    return
    {
        JsonResponsePacketSerializer::serializeResponse(response),
        this->getFactorySafely()->createRoomAdminRequestHandler(m_user, addedRoomData.id)
    };
}

std::shared_ptr<RequestHandlerFactory> MenuRequestHandler::getFactorySafely()
{
    if (auto factory = m_handlerFactory.lock())
    {
        return factory;
    }

    throw ServerException("MenuRequestHandler: [ERROR]: RequestHandlerFactory is no longer available");
}

unsigned int MenuRequestHandler::generateNewRoomId()
{
    return m_currentRoomId.fetch_add(1, std::memory_order_relaxed);
}

#include "JsonResponsePacketSerializer.h"
#include <nlohmann/json.hpp>

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const ErrorResponse& errorResponse)
{
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"message", errorResponse.message}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::ERROR_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const LoginResponse& loginResponse)
{
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"status", loginResponse.status}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::LOGIN_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const SignupResponse& signUpResponse)
{
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"status", signUpResponse.status}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::SIGNUP_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const LogoutResponse& loguotResponse)
{
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"status", loguotResponse.status}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::SIGNOUT_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetRoomsResponse& getRoomsResponse) 
{
    // Convert each RoomData object to JSON format manually
    nlohmann::json jsonRooms = nlohmann::json::array();
    for (const auto& room : getRoomsResponse.rooms) 
    {
        jsonRooms.push_back({
            {"id", room.id},
            {"name", room.name},
            {"maxPlayers", room.maxPlayers},
            {"numOfQuestionsInGame", room.numOfQuestionsInGame},
            {"timePerQuestion", room.timePerQuestion},
            {"status", room.status}
            });
    }
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"status", getRoomsResponse.status}, {"rooms", jsonRooms}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::GET_ROOMS_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetPlayersInRoomResponse& getPlayersInRoomResponse) 
{
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"players", getPlayersInRoomResponse.players}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::GET_ROOMS_PLAYERS_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const JoinRoomResponse& joinRoomResponse) 
{
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"status", joinRoomResponse.status}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::JOIN_ROOM_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const CreateRoomResponse& createRoomResponse) 
{
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"status", createRoomResponse.status}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::CREATE_ROOM_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const getHighScoreResponse& getHighScoreResponse) 
{
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"status", getHighScoreResponse.status}, {"statistics", getHighScoreResponse.statistics}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::GET_HIGH_SCORE_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const getPersonalStatsResponse& getPersonalStatsResponse) 
{
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"status", getPersonalStatsResponse.status}, {"statistics", getPersonalStatsResponse.statistics}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::GET_PERSONAL_STATISTICS_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const CloseRoomResponse& closeRoomResponse)
{
    nlohmann::json jsonResponse = { { "status", closeRoomResponse.status } };
    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::CLOSE_ROOM_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const StartGameResponse& startGameResponse)
{
    nlohmann::json jsonResponse = { { "status", startGameResponse.status } };
    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::START_GAME_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetRoomStateResponse& getRoomStateResponse)
{
    nlohmann::json jsonResponse = {
        {"status", getRoomStateResponse.status},
        {"hasGameBegun", getRoomStateResponse.hasGameBegun},
        {"players", getRoomStateResponse.players},
        {"questionCount", getRoomStateResponse.questionsCount},
        {"answerTimeout", getRoomStateResponse.answerTimeout}
    };

    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::GET_ROOM_STATE_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const LeaveRoomResponse& leaveRoomResponse)
{
    nlohmann::json jsonResponse = { { "status", leaveRoomResponse.status } };
    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::LEAVE_ROOM_RESPONSE, jsonString);
}

void JsonResponsePacketSerializer::insertLengthAsBytes(std::vector<unsigned char>& buffer, const int dataLength)
{
    const int SHIFT_BYTE_3 = 24;
    const int SHIFT_BYTE_2 = 16;
    const int SHIFT_BYTE_1 = 8;
    const int SHIFT_BYTE_0 = 0;

    buffer.push_back((dataLength >> SHIFT_BYTE_3) & 0xFF);
    buffer.push_back((dataLength >> SHIFT_BYTE_2) & 0xFF);
    buffer.push_back((dataLength >> SHIFT_BYTE_1) & 0xFF);
    buffer.push_back((dataLength >> SHIFT_BYTE_0) & 0xFF);
}

std::vector<unsigned char> JsonResponsePacketSerializer::buildResponse(const ResponseCode code, const std::string& jsonString)
{
    std::vector<unsigned char> buffer;

    buffer.push_back(static_cast<unsigned char>(code));

    // Adding message length using Big Endian
    insertLengthAsBytes(buffer, jsonString.size());

    buffer.insert(buffer.end(), jsonString.begin(), jsonString.end());

    return buffer;
}

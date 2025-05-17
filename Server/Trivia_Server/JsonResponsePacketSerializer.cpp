#include "JsonResponsePacketSerializer.h"
#include <nlohmann/json.hpp>

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const ErrorResponse& errorResponse)
{
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"message", errorResponse.message}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return std::vector<unsigned char>(jsonString.cbegin(), jsonString.cend());
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const LoginResponse& loginResponse)
{
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"status", loginResponse.status}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return std::vector<unsigned char>(jsonString.cbegin(), jsonString.cend());
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const SignupResponse& signUpResponse)
{
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"status", signUpResponse.status}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return std::vector<unsigned char>(jsonString.cbegin(), jsonString.cend());
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const LogoutResponse& loguotResponse)
{
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"status", loguotResponse.status}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return std::vector<unsigned char>(jsonString.cbegin(), jsonString.cend());
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
    return std::vector<unsigned char>(jsonString.cbegin(), jsonString.cend());
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetPlayersInRoomResponse& getPlayersInRoomResponse) 
{
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"players", getPlayersInRoomResponse.players}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return std::vector<unsigned char>(jsonString.cbegin(), jsonString.cend());
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const JoinRoomResponse& joinRoomResponse) 
{
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"status", joinRoomResponse.status}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return std::vector<unsigned char>(jsonString.cbegin(), jsonString.cend());
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const CreateRoomResponse& createRoomResponse) 
{
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"status", createRoomResponse.status}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return std::vector<unsigned char>(jsonString.cbegin(), jsonString.cend());
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const getHighScoreResponse& getHighScoreResponse) 
{
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"status", getHighScoreResponse.status}, {"statistics", getHighScoreResponse.statistics}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return std::vector<unsigned char>(jsonString.cbegin(), jsonString.cend());
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const getPersonalStatsResponse& getPersonalStatsResponse) 
{
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"status", getPersonalStatsResponse.status}, {"statistics", getPersonalStatsResponse.statistics}};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return std::vector<unsigned char>(jsonString.begin(), jsonString.end());
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const CloseRoomResponse& closeRoomResponse)
{
    nlohmann::json jsonResponse = { { "status", closeRoomResponse.status } };
    std::string jsonString = jsonResponse.dump();
    return std::vector<unsigned char>(jsonString.cbegin(), jsonString.cend());
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const StartGameResponse& startGameResponse)
{
    nlohmann::json jsonResponse = { { "status", startGameResponse.status } };
    std::string jsonString = jsonResponse.dump();
    return std::vector<unsigned char>(jsonString.cbegin(), jsonString.cend());
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

    std::string jsonStr = jsonResponse.dump();
    std::vector<unsigned char> buffer(jsonStr.cbegin(), jsonStr.cend());
    return buffer;
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const LeaveRoomResponse& leaveRoomResponse)
{
    nlohmann::json jsonResponse = { { "status", leaveRoomResponse.status } };
    std::string jsonString = jsonResponse.dump();
    return std::vector<unsigned char>(jsonString.cbegin(), jsonString.cend());
}

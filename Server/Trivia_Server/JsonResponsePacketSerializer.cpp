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
            {"timePerQuestion", room.timePerQuestion}
            //{"status", room.status}
            });
    }
    // Creating the JSON Object
    nlohmann::json jsonResponse = {{"status", getRoomsResponse.status}, jsonRooms};

    // Converting the JSON to a string and returning vector of bytes by it's iterators
    std::string jsonString = jsonResponse.dump();
    return std::vector<unsigned char>(jsonString.cbegin(), jsonString.cend());
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializerResponse(const GetPlayersInRoomResponse& getPlayersInRoomResponse) 
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
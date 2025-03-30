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

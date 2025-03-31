#include "JsonRequestPacketDeserializer.h"
#include <nlohmann/json.hpp>

LoginRequest JsonRequestPacketDeserializer::deserializeLoginRequest(const std::vector<unsigned char>& buffer)
{
    const std::string USER_NAME = "username", PASSWORD = "password";

    // Convert the buffer to a string
    std::string jsonStr(buffer.cbegin(), buffer.cend());

    // Parse the json
    auto jsonData = nlohmann::json::parse(jsonStr, nullptr, false);

    // Get the info to loginRequest and return it
    return LoginRequest
    {
        jsonData[USER_NAME].get<std::string>(),
        jsonData[PASSWORD].get<std::string>()
    };
}

SignupRequest JsonRequestPacketDeserializer::deserializeSignupRequest(const std::vector<unsigned char>& buffer)
{
    const std::string USER_NAME = "username", PASSWORD = "password", EMAIL = "email";

    // Convert the buffer to a string
    std::string jsonStr(buffer.cbegin(), buffer.cend());

    // Parse the json
    auto jsonData = nlohmann::json::parse(jsonStr, nullptr, false);

    // Get the info to SignupRequest and return it
    return SignupRequest
    {
        jsonData[USER_NAME].get<std::string>(),
        jsonData[PASSWORD].get<std::string>(),
        jsonData[EMAIL].get<std::string>()
    };
}

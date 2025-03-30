#include "JsonRequestPacketDeserializer.h"
#include <nlohmann/json.hpp>

LoginRequest JsonRequestPacketDeserializer::deserializeLoginRequest(const std::vector<unsigned char>& buffer)
{
    // Convert the buffer to a string
    std::string jsonStr(buffer.begin(), buffer.end());

    // Parse the json
    auto jsonData = nlohmann::json::parse(jsonStr, nullptr, false);

    // Get the info to loginRequest and return it
    return LoginRequest
    {
        jsonData["username"].get<std::string>(),
        jsonData["password"].get<std::string>()
    };
}

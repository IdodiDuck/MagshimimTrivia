#include "JsonRequestPacketDeserializer.h"
#include <nlohmann/json.hpp>
#include <iostream>

std::optional<LoginRequest> JsonRequestPacketDeserializer::deserializeLoginRequest(const std::vector<unsigned char>& buffer)
{
    const std::string USER_NAME = "username", PASSWORD = "password";

    // Convert the buffer to a string
    std::string jsonStr(buffer.cbegin(), buffer.cend());

    // Parse the JSON
    auto jsonData = nlohmann::json::parse(jsonStr, nullptr, false);

    // Get the info to loginRequest and return it
    if (jsonData.is_discarded()) 
    {
        return {};
    }

    try
    {
        auto username = jsonData[USER_NAME].get<std::string>();
        auto password = jsonData[PASSWORD].get<std::string>();

        if ((username.empty()) || (password.empty()))
        {
            throw std::exception("Error: failed to extract json data");
        }

        return LoginRequest {username, password};
    }

    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return {};
    }
}

std::optional<SignupRequest> JsonRequestPacketDeserializer::deserializeSignupRequest(const std::vector<unsigned char>& buffer)
{
    const std::string USER_NAME = "username", PASSWORD = "password", EMAIL = "email";

    // Convert the buffer to a string
    std::string jsonStr(buffer.cbegin(), buffer.cend());

    // Parse the JSON
    auto jsonData = nlohmann::json::parse(jsonStr, nullptr, false);

    // Get the info to signupRequest and return it
    if (jsonData.is_discarded())
    {
        return {};
    }

    try
    {
        auto username = jsonData[USER_NAME].get<std::string>();
        auto password = jsonData[PASSWORD].get<std::string>();
        auto email = jsonData[EMAIL].get<std::string>();

        if ((username.empty()) || (password.empty()) || (email.empty()))
        {
            throw std::exception("Error: Failed to extract JSON data");
        }

        return SignupRequest{ username, password, email};
    }

    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return {};
    }
}

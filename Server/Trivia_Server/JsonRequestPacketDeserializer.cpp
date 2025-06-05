#include "JsonRequestPacketDeserializer.h"
#include <nlohmann/json.hpp>
#include <iostream>

std::optional<LoginRequest> JsonRequestPacketDeserializer::deserializeLoginRequest(const std::vector<unsigned char>& buffer)
{
    const std::string USER_NAME = "username", PASSWORD = "password";

    std::string jsonStr(buffer.cbegin(), buffer.cend());

    auto jsonData = nlohmann::json::parse(jsonStr, nullptr, false);

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

        return LoginRequest{ username, password };
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

    std::string jsonStr(buffer.cbegin(), buffer.cend());

    auto jsonData = nlohmann::json::parse(jsonStr, nullptr, false);

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

        return SignupRequest{ username, password, email };
    }

    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return {};
    }
}

std::optional<GetPlayersInRoomRequest> JsonRequestPacketDeserializer::deserializeGetPlayersRequest(const std::vector<unsigned char>& buffer)
{
    const std::string ROOM_ID = "roomId";

    std::string jsonStr(buffer.begin(), buffer.end());

    auto jsonData = nlohmann::json::parse(jsonStr, nullptr, false);

    if (jsonData.is_discarded())
    {
        std::cerr << "Error: Failed to parse JSON data for GetPlayersInRoomRequest." << std::endl;
        return {};
    }

    try
    {
        unsigned int roomId = jsonData.at(ROOM_ID).get<unsigned int>();

        return GetPlayersInRoomRequest{ roomId };
    }

    catch (const std::exception& e)
    {
        std::cerr << "Error extracting GetPlayersInRoomRequest data: " << e.what() << std::endl;
        return {};
    }
}

std::optional<JoinRoomRequest> JsonRequestPacketDeserializer::deserializeJoinRoomRequest(const std::vector<unsigned char>& buffer)
{
    const std::string ROOM_ID = "roomId";

    std::string jsonStr(buffer.begin(), buffer.end());

    auto jsonData = nlohmann::json::parse(jsonStr, nullptr, false);

    if (jsonData.is_discarded())
    {
        std::cerr << "Error: Failed to parse JSON data for GetPlayersInRoomRequest." << std::endl;
        return JoinRoomRequest{ 0 };
    }

    try
    {
        unsigned int roomId = jsonData.at(ROOM_ID).get<unsigned int>();

        return JoinRoomRequest{ roomId };
    }

    catch (const std::exception& e)
    {
        std::cerr << "Error extracting GetPlayersInRoomRequest data: " << e.what() << std::endl;
        return  JoinRoomRequest{ 0 };
    }
}

std::optional<CreateRoomRequest> JsonRequestPacketDeserializer::deserializeCreateRoomRequest(const std::vector<unsigned char>& buffer)
{
    const std::string ROOM_NAME = "roomName", MAX_USERS = "maxUsers", QUESTION_COUNT = "questionCount", ANSWER_TIMEOUT = "answerTimeout";

    std::string jsonStr(buffer.begin(), buffer.end());

    auto jsonData = nlohmann::json::parse(jsonStr, nullptr, false);

    if (jsonData.is_discarded())
    {
        std::cerr << "Error: Failed to parse JSON data for GetPlayersInRoomRequest." << std::endl;
        return CreateRoomRequest{ "", 0, 0, 0 };
    }

    try
    {
        std::string roomName = jsonData.at(ROOM_NAME).get<std::string>();
        unsigned int maxUsers = jsonData.at(MAX_USERS).get<unsigned int>();
        unsigned int questionCount = jsonData.at(QUESTION_COUNT).get<unsigned int>();
        unsigned int answerTimeout = jsonData.at(ANSWER_TIMEOUT).get<unsigned int>();

        return CreateRoomRequest{ roomName, maxUsers, questionCount, answerTimeout };
    }

    catch (const std::exception& e)
    {
        std::cerr << "Error extracting GetPlayersInRoomRequest data: " << e.what() << std::endl;
        return CreateRoomRequest{ "", 0, 0, 0 };
    }
}

std::optional<SubmitAnswerRequest> JsonRequestPacketDeserializer::deserializeSubmitAnswerRequest(const std::vector<unsigned char>& buffer)
{
    const std::string ANSWER_ID = "answerId";
    const std::string ANSWER_TIME = "answerTime";

    std::string jsonStr(buffer.begin(), buffer.end());

    auto jsonData = nlohmann::json::parse(jsonStr, nullptr, false);

    if (jsonData.is_discarded())
    {
        std::cerr << "Error: Failed to parse JSON data for SubmitAnswerRequest." << std::endl;
        return { };
    }

    try
    {
        unsigned int answerId = jsonData.at(ANSWER_ID).get<unsigned int>();
        double answerTime = jsonData.at(ANSWER_TIME).get<double>();

        return SubmitAnswerRequest{ answerId, answerTime };
    }

    catch (const std::exception& e)
    {
        std::cerr << "Error extracting SubmitAnswerRequest data: " << e.what() << std::endl;
        return {};
    }

}

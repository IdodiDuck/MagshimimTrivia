#include "JsonResponsePacketSerializer.h"
#include <nlohmann/json.hpp>

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const ErrorResponse& errorResponse)
{
    nlohmann::json jsonResponse = {{"message", errorResponse.message}};

    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::ERROR_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const LoginResponse& loginResponse)
{
    nlohmann::json jsonResponse = {{"status", loginResponse.status}};

    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::LOGIN_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const SignupResponse& signUpResponse)
{
    nlohmann::json jsonResponse = {{"status", signUpResponse.status}};

    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::SIGNUP_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const LogoutResponse& loguotResponse)
{
    nlohmann::json jsonResponse = {{"status", loguotResponse.status}};

    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::SIGNOUT_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetRoomsResponse& getRoomsResponse) 
{
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
    nlohmann::json jsonResponse = {{"status", getRoomsResponse.status}, {"rooms", jsonRooms}};

    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::GET_ROOMS_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetPlayersInRoomResponse& getPlayersInRoomResponse) 
{
    nlohmann::json jsonResponse = {{"players", getPlayersInRoomResponse.players}};

    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::GET_ROOMS_PLAYERS_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const JoinRoomResponse& joinRoomResponse) 
{
    nlohmann::json jsonResponse = {{"status", joinRoomResponse.status}};

    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::JOIN_ROOM_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const CreateRoomResponse& createRoomResponse) 
{
    nlohmann::json jsonResponse = {{"status", createRoomResponse.status}};

    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::CREATE_ROOM_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const getHighScoreResponse& getHighScoreResponse) 
{
    nlohmann::json jsonResponse = {{"status", getHighScoreResponse.status}, {"statistics", getHighScoreResponse.statistics}};

    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::GET_HIGH_SCORE_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const getPersonalStatsResponse& getPersonalStatsResponse) 
{
    nlohmann::json jsonResponse = {{"status", getPersonalStatsResponse.status}, {"statistics", getPersonalStatsResponse.statistics}};

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

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetGameResultsResponse& getGameResultsResponse)
{
    nlohmann::json jsonResponse = { {"status", getGameResultsResponse.status}, {"results", nlohmann::json::array()} };

    for (const auto& playerResult : getGameResultsResponse.results)
    {
        jsonResponse["results"].push_back({
            {"username", playerResult.username},
            {"correctAnswerCount", playerResult.correctAnswerCount},
            {"wrongAnswerCount", playerResult.wrongAnswerCount},
            {"averageAnswerTime", playerResult.averageAnswerTime}
            });
    }

    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::GET_GAME_RESULTS_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const SubmitAnswerResponse& submitAnswerResponse)
{
    nlohmann::json jsonResponse = { {"status", submitAnswerResponse.status}, {"correctAnswerId", submitAnswerResponse.correctAnswerId} };
    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::SUBMIT_ANSWER_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const GetQuestionResponse& getQuestionResponse)
{
    nlohmann::json answersJson;
    for (const auto& [key, value] : getQuestionResponse.answers)
    {
        answersJson[std::to_string(key)] = value;
    }

    nlohmann::json jsonResponse = {
        {"status", getQuestionResponse.status},
        {"question", getQuestionResponse.question},
        {"answers", answersJson}
    };

    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::GET_QUESTION_RESPONSE, jsonString);
}

std::vector<unsigned char> JsonResponsePacketSerializer::serializeResponse(const LeaveGameResponse& leaveGameResponse)
{
    nlohmann::json jsonResponse = { {"status", leaveGameResponse.status} };
    std::string jsonString = jsonResponse.dump();
    return buildResponse(ResponseCode::LEAVE_GAME_RESPONSE, jsonString);
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

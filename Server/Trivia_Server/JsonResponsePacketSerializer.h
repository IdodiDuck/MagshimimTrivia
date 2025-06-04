#pragma once

#include <vector>

#include "Constants.h"
#include "Room.h"
#include <unordered_map>

// V1.0.0 Responses - 
typedef struct LoginResponse
{
    unsigned int status;

} LoginResponse;

typedef struct SignupResponse
{
    unsigned int status;

} SignupResponse;

typedef struct ErrorResponse
{
    std::string message;

} ErrorResponse;

// V2.0.0 Responses - 

typedef struct LogoutResponse
{
    unsigned int status;

} LogoutResponse;

typedef struct GetRoomsResponse
{
    unsigned int status;
    std::vector<RoomData> rooms;

} GetRoomsResponse;

typedef struct GetPlayersInRoomResponse
{
    std::vector<std::string> players;

} GetPlayersInRoomResponse;

typedef struct getHighScoreResponse
{
    unsigned int status;
    std::vector<std::string> statistics;

} getHighScoreResponse;

typedef struct getPersonalStatsResponse
{
    unsigned int status;
    std::vector<std::string> statistics;

} getPersonalStatsResponse;

typedef struct JoinRoomResponse
{
    unsigned int status;

} JoinRoomResponse;

typedef struct CreateRoomResponse
{
    unsigned int status;

} CreateRoomResponse;

// V3.0.0 Responses - 
typedef struct CloseRoomResponse
{
    unsigned int status;

} CloseRoomResponse;

typedef struct StartGameResponse
{
    unsigned int status;

} StartGameResponse;

typedef struct GetRoomStateResponse
{
    unsigned int status;
    bool hasGameBegun;
    std::vector<std::string> players;
    unsigned int questionsCount;
    unsigned int answerTimeout;


} GetRoomStateResponse;

typedef struct LeaveRoomResponse
{
    unsigned int status;

} LeaveRoomResponse;

// V4.0.0 Responses - 
typedef struct LeaveGameResponse
{
    unsigned int status;

} LeaveGameResponse;

typedef struct GetQuestionResponse
{
    unsigned int status;
    std::string question;
    std::unordered_map<unsigned int, std::string> answers;

} GetQuestionResponse;

typedef struct SubmitAnswerResponse
{
    unsigned int status;
    unsigned int correctAnswerId;

} SubmitAnswerResponse;


typedef struct PlayerResults
{
    std::string username;
    unsigned int correctAnswerCount;
    unsigned int wrongAnswerCount;
    double averageAnswerTime;

} PlayerResults;

typedef struct GetGameResultsResponse
{
    unsigned int status;
    std::vector<PlayerResults> results;

} GetGameResultsResponse;

class JsonResponsePacketSerializer
{

public:
    // V1.0.0 Serializations - 
    static std::vector<unsigned char> serializeResponse(const ErrorResponse& errorResponse);
    static std::vector<unsigned char> serializeResponse(const LoginResponse& loginResponse);
    static std::vector<unsigned char> serializeResponse(const SignupResponse& signUpResponse);

    // V2.0.0 Serializations - 
    static std::vector<unsigned char> serializeResponse(const LogoutResponse& loguotResponse);
    static std::vector<unsigned char> serializeResponse(const GetRoomsResponse& getRoomsResponse);
    static std::vector<unsigned char> serializeResponse(const GetPlayersInRoomResponse& getPlayersInRoomResponse);
    static std::vector<unsigned char> serializeResponse(const JoinRoomResponse& joinRoomResponse);
    static std::vector<unsigned char> serializeResponse(const CreateRoomResponse& createRoomResponse);
    static std::vector<unsigned char> serializeResponse(const getHighScoreResponse& getHighScoreResponse);
    static std::vector<unsigned char> serializeResponse(const getPersonalStatsResponse& getPersonalStatsResponse);

    // V3.0.0 Serializations - 
    static std::vector<unsigned char> serializeResponse(const CloseRoomResponse& closeRoomResponse);
    static std::vector<unsigned char> serializeResponse(const StartGameResponse& startGameResponse);
    static std::vector<unsigned char> serializeResponse(const GetRoomStateResponse& getRoomStateResponse);
    static std::vector<unsigned char> serializeResponse(const LeaveRoomResponse& leaveRoomResponse);

    // V4.0.0 Serializations - 
    static std::vector<unsigned char> serializeResponse(const GetGameResultsResponse& getGameResultsResponse);
    static std::vector<unsigned char> serializeResponse(const SubmitAnswerResponse& submitAnswerResponse);
    static std::vector<unsigned char> serializeResponse(const GetQuestionResponse& getQuestionResponse);
    static std::vector<unsigned char> serializeResponse(const LeaveGameResponse& leaveGameResponse);

private:
    // Support Methods - 
    static void insertLengthAsBytes(std::vector<unsigned char>& buffer, const int dataLength);
    static std::vector<unsigned char> buildResponse(const ResponseCode code, const std::string& jsonString);
};
#pragma once

#include <vector>

#include "Constants.h"
#include "Room.h"

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

// v2.0.0

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

class JsonResponsePacketSerializer
{

public:
    static std::vector<unsigned char> serializeResponse(const ErrorResponse& errorResponse);
    static std::vector<unsigned char> serializeResponse(const LoginResponse& loginResponse);
    static std::vector<unsigned char> serializeResponse(const SignupResponse& signUpResponse);

    static std::vector<unsigned char> serializeResponse(const LogoutResponse& loguotResponse);
    static std::vector<unsigned char> serializeResponse(const GetRoomsResponse& getRoomsResponse);
    static std::vector<unsigned char> serializerResponse(const GetPlayersInRoomResponse& getPlayersInRoomResponse);
    static std::vector<unsigned char> serializeResponse(const JoinRoomResponse& joinRoomResponse);
    static std::vector<unsigned char> serializeResponse(const CreateRoomResponse& createRoomResponse);
    static std::vector<unsigned char> serializeResponse(const getHighScoreResponse& getHighScoreResponse);
    static std::vector<unsigned char> serializeResponse(const getPersonalStatsResponse& getPersonalStatsResponse);
};
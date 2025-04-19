#pragma once

#include <vector>

#include "Constants.h"

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
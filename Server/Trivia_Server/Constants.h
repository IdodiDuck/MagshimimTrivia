#pragma once

#include <string>
#include <vector>

// =============================
// Network Constants
// =============================

const std::string SERVER_IP_ADDRESS = "127.0.0.1";
const int SERVER_PORT = 8820;

const std::string EXIT_SERVER = "EXIT";
const int BUFFER_SIZE = 10; // Remove/Modify later

// =============================
// Request Codes Constants
// =============================

enum class RequestCode : unsigned int 
{
    SIGNUP_REQUEST = 100,
    LOGIN_REQUEST = 101
};

// =============================
// Status Codes Constants
// =============================

const unsigned int SUCCESS = 0;
const unsigned int FAILURE = 1;

// =============================
// Response Structs
// =============================

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

typedef struct RoomData // relocate to room.h when added
{
    unsigned int id;
    std::string name;
    unsigned int maxPlayers;
    unsigned int numOfQuestionsInGame;
    unsigned int timePerQuestion;
    //RoomStatus status;

} RoomData;

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

// =============================
// Error Codes Constants
// =============================

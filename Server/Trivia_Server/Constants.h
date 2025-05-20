#pragma once

#include <string>
#include <vector>

// =============================
// Network Constants
// =============================

const std::string SERVER_IP_ADDRESS = "127.0.0.1";
const int SERVER_PORT = 8820;

const std::string EXIT_SERVER = "EXIT";

// =============================
// Request Codes Constants
// =============================

enum class RequestCode : unsigned int 
{
    SIGNUP_REQUEST = 10,
    LOGIN_REQUEST,

    SIGNOUT_REQUEST = 20,
    ROOMS_REQUEST,
    PLAYERS_IN_ROOM_REQUEST,
    PERSONAL_STATS_REQUEST,
    HIGH_SCORE_REQUEST,
    JOIN_ROOM_REQUEST,
    CREATE_ROOM_REQUEST,

    CLOSE_ROOM_REQUEST = 30,
    START_GAME_REQUEST,
    GET_ROOM_STATE_REQUEST,
    LEAVE_ROOM_REQUEST

};

// =============================
// Responses Codes Constants
// =============================

enum class ResponseCode : unsigned int
{
    ERROR_RESPONSE = 99,
    SIGNUP_RESPONSE = 10,
    LOGIN_RESPONSE,

    SIGNOUT_RESPONSE = 20,
    ROOMS_RESPONSE,
    PLAYERS_IN_ROOM_RESPONSE,
    PERSONAL_STATS_RESPONSE,
    HIGH_SCORE_RESPONSE,
    JOIN_ROOM_RESPONSE,
    CREATE_ROOM_RESPONSE,
    GET_ROOMS_RESPONSE,
    GET_ROOMS_PLAYERS_RESPONSE,
    GET_HIGH_SCORE_RESPONSE,
    GET_PERSONAL_STATISTICS_RESPONSE,

    CLOSE_ROOM_RESPONSE = 30,
    START_GAME_RESPONSE,
    GET_ROOM_STATE_RESPONSE,
    LEAVE_ROOM_RESPONSE,

    LEAVE_GAME_RESPONSE = 40,
    GET_QUESTION_RESPONSE,
    SUBMIT_ANSWER_RESPONSE,
    GET_GAME_RESULTS_RESPONSE

};

// =============================
// Status Codes Constants
// =============================

const unsigned int SUCCESS = 0;
const unsigned int FAILURE = 1;

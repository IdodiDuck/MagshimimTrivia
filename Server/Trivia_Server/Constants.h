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
    LOGIN_REQUEST,

    SIGNOUT_REQUEST = 200,
    ROOMS_REQUEST,
    PLAYERS_IN_ROOM_REQUEST,
    PERSONAL_STATS_REQUEST,
    HIGH_SCORE_REQUEST,
    JOIN_ROOM_REQUEST,
    CREATE_ROOM_REQUEST

};

// =============================
// Status Codes Constants
// =============================

const unsigned int SUCCESS = 0;
const unsigned int FAILURE = 1;

// =============================
// Error Codes Constants
// =============================

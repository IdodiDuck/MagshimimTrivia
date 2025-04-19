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
// Error Codes Constants
// =============================

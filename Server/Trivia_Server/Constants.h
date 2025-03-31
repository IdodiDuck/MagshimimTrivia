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

const unsigned int SIGNUP_REQUEST_CODE = 100;
const unsigned int LOGIN_REQUEST_CODE = 101;

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

// =============================
// Error Codes Constants
// =============================

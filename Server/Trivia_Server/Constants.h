#pragma once

#include <string>

// =============================
// Network Constants
// =============================

const std::string SERVER_IP_ADDRESS = "127.0.0.1";
const int SERVER_PORT = 8820;

const std::string EXIT_SERVER = "EXIT";
const int BUFFER_SIZE = 10;

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

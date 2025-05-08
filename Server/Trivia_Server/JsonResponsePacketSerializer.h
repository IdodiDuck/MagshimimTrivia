#pragma once

#include <vector>

#include "Constants.h"

class JsonResponsePacketSerializer
{

public:
    static std::vector<unsigned char> serializeResponse(const ErrorResponse& errorResponse);
    static std::vector<unsigned char> serializeResponse(const LoginResponse& loginResponse);
    static std::vector<unsigned char> serializeResponse(const SignupResponse& signUpResponse);
};
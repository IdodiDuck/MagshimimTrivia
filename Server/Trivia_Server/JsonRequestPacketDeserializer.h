#pragma once

#include <vector>
#include <string>
#include <optional>

// ===========================
// Request Structs
// ===========================

typedef struct LoginRequest
{
	std::string username;
	std::string password;

} LoginRequest;

typedef struct SignupRequest
{
	std::string username;
	std::string password;
	std::string email;

} SignupRequest;



class JsonRequestPacketDeserializer
{
public: 
	static std::optional<LoginRequest> deserializeLoginRequest(const std::vector<unsigned char>& buffer);
	static std::optional<SignupRequest> deserializeSignupRequest(const std::vector<unsigned char>& buffer);

};


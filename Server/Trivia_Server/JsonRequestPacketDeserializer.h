#pragma once

#include <vector>
#include <string>
#include <optional>

// ===========================
// Request Structs
// ===========================

// Login and SignUp Structs (V1) - 
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

// Room Structs (V2) - 
typedef struct GetPlayersInRoomRequest
{
	unsigned int roomId;

} GetPlayersInRoomRequest;

typedef struct JoinRoomRequest
{
	unsigned int roomId;

} JoinRoomRequest;

typedef struct CreateRoomRequest
{
	std::string roomName;
	unsigned int maxUsers;
	unsigned int questionCount;
	unsigned int answerTimeout;

} CreateRoomRequest;


class JsonRequestPacketDeserializer
{
public: 
	static std::optional<LoginRequest> deserializeLoginRequest(const std::vector<unsigned char>& buffer);
	static std::optional<SignupRequest> deserializeSignupRequest(const std::vector<unsigned char>& buffer);

};


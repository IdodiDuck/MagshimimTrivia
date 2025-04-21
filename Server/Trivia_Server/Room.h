#pragma once

#include "LoggedUser.h"

#include <vector>
#include <string>

enum class RoomStatus : unsigned int
{
    OPEN = 0,
    CLOSED
};

typedef struct RoomData 
{
    unsigned int id;
    std::string name;
    unsigned int maxPlayers;
    unsigned int numOfQuestionsInGame;
    unsigned int timePerQuestion;
    RoomStatus status;

} RoomData;

class Room
{

public:
    void addUser(LoggedUser user);
    void removeUser(LoggedUser);
    std::vector<std::string> getAllUsers();

private:
    RoomData m_metadata;
    std::vector<LoggedUser> m_users;

};


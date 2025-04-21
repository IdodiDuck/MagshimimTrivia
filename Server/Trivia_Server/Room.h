#pragma once

#include "LoggedUser.h"

#include <set>
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
    void removeUser(LoggedUser user);
    std::set<std::string> getAllUsers();

private:
    RoomData m_metadata;
    std::set<LoggedUser> m_users;

};


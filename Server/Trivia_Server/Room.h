#pragma once

#include "LoggedUser.h"

#include <set>
#include <string>

enum class RoomStatus : unsigned int
{
    OPENED = 0,
    CLOSED,
    GAME_STARTED
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
    Room(const RoomData& roomData);
    ~Room();

    // Setters - 
    void addUser(const LoggedUser& user);
    void removeUser(const LoggedUser& user);
    void changeRoomStatus(const RoomStatus& newStatus);

    // Getters - 
    std::set<std::string> getAllUsers() const;
    RoomData getRoomData() const;
    bool isRoomFull() const;
    

private:
    // Attributes - 
    RoomData m_metadata;
    std::set<LoggedUser> m_users;

};


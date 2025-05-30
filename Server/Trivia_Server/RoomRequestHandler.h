#pragma once

#include "IRequestHandler.h"
#include "Room.h"
#include "RoomManager.h"

class RequestHandlerFactory;

class RoomRequestHandler : public IRequestHandler
{

protected:
    // C'tor - 
    RoomRequestHandler(std::weak_ptr<RequestHandlerFactory> handlerFactory, RoomManager& roomManager, const LoggedUser& loggedUser, const Room& room, const bool isAdmin);
    
    // Getters - 
    std::shared_ptr<RequestHandlerFactory> getFactorySafely();
    Room& getRoomSafely();

    RequestResult getRoomState(const RequestInfo& info);

    // Attributes - 
    std::weak_ptr<RequestHandlerFactory> m_handlerFactory;
    RoomManager& m_roomManager;
    LoggedUser m_user;
    Room m_room;

private:
    bool _isAdmin;
};


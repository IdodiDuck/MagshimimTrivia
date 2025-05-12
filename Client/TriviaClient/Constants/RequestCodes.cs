using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TriviaClient.Constants
{
    public enum RequestCode : uint
    {
        // Authuentication Requests - 
        SignupRequest = 100,
        LoginRequest,

        // General Requests - 
        SignoutRequest = 200,
        RoomsRequest,
        PlayersInRoomRequest,
        PersonalStatsRequest,
        HighScoreRequest,
        JoinRoomRequest,
        CreateRoomRequest,
        
        // Room Requests - 
        CLOSE_ROOM_REQUEST = 300,
        START_GAME_REQUEST,
        GET_ROOM_STATE_REQUEST,
        LEAVE_ROOM_REQUEST
    }
}

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
        CreateRoomRequest
    }
}

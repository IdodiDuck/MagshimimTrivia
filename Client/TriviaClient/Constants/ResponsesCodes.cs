using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TriviaClient.Constants
{
    public enum ResponseCode : uint
    {
        ERROR_RESPONSE = 99,

        SIGNUP_RESPONSE = 10,
        LOGIN_RESPONSE,

        SIGNOUT_RESPONSE = 20,
        ROOMS_RESPONSE,
        PLAYERS_IN_ROOM_RESPONSE,
        PERSONAL_STATS_RESPONSE,
        HIGH_SCORE_RESPONSE,
        JOIN_ROOM_RESPONSE,
        CREATE_ROOM_RESPONSE,
        GET_ROOMS_RESPONSE,
        GET_ROOMS_PLAYERS_RESPONSE,
        GET_HIGH_SCORE_RESPONSE,
        GET_PERSONAL_STATISTICS_RESPONSE,

        CLOSE_ROOM_RESPONSE = 30,
        START_GAME_RESPONSE,
        GET_ROOM_STATE_RESPONSE,
        LEAVE_ROOM_RESPONSE
    }
}

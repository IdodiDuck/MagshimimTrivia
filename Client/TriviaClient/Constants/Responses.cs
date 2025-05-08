using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TriviaClient.Constants
{
    internal class Responses
    {
        public class ErrorResponse
        {
            public string Message { get; set; } = string.Empty;
        }

        public class LoginResponse
        {
            public uint Status { get; set; }
        }

        public class SignupResponse
        {
            public uint Status { get; set; }
        }

        public class LogoutResponse
        {
            public uint Status { get; set; }
        }

        public class RoomData
        {
            public uint Id { get; set; }
            public string Name { get; set; } = string.Empty;
            public uint MaxPlayers { get; set; }
            public uint NumOfQuestionsInGame { get; set; }
            public uint TimePerQuestion { get; set; }
            public uint Status { get; set; }
        }

        public class GetRoomsResponse
        {
            public uint Status { get; set; }
            public List<RoomData> Rooms { get; set; } = new List<RoomData>();
        }

        public class GetPlayersInRoomResponse
        {
            public List<string> Players { get; set; } = new List<string>();
        }

        public class JoinRoomResponse
        {
            public uint Status { get; set; }
        }

        public class CreateRoomResponse
        {
            public uint Status { get; set; }
        }

        public class GetHighScoreResponse
        {
            public uint Status { get; set; }
            public List<string> Statistics { get; set; } = new List<string>();
        }

        public class GetPersonalStatsResponse
        {
            public uint Status { get; set; }
            public List<string> Statistics { get; set; } = new List<string>();
        }

    }
}

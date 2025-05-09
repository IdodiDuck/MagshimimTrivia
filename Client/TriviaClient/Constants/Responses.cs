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
            public string message { get; set; } = string.Empty;
        }

        public class LoginResponse
        {
            public uint status { get; set; }
        }

        public class SignupResponse
        {
            public uint status { get; set; }
        }

        public class LogoutResponse
        {
            public uint status { get; set; }
        }

        public class RoomData
        {
            public uint id { get; set; }
            public string name { get; set; } = string.Empty;
            public uint maxPlayers { get; set; }
            public uint numOfQuestionsInGame { get; set; }
            public uint timePerQuestion { get; set; }
            public uint status { get; set; }
        }

        public class GetRoomsResponse
        {
            public uint status { get; set; }
            public List<RoomData> rooms { get; set; } = new List<RoomData>();
        }

        public class GetPlayersInRoomResponse
        {
            public List<string> players { get; set; } = new List<string>();
        }

        public class JoinRoomResponse
        {
            public uint status { get; set; }
        }

        public class CreateRoomResponse
        {
            public uint status { get; set; }
        }

        public class GetHighScoreResponse
        {
            public uint status { get; set; }
            public List<string> statistics { get; set; } = new List<string>();
        }

        public class GetPersonalStatsResponse
        {
            public uint status { get; set; }
            public List<string> statistics { get; set; } = new List<string>();
        }

    }
}

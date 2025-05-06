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
            public string Message { get; set; }
        }

        public class LoginResponse
        {
            public int Status { get; set; }
        }

        public class SignupResponse
        {
            public int Status { get; set; }
        }

        public class LogoutResponse
        {
            public int Status { get; set; }
        }

        public class RoomData
        {
            public int Id { get; set; }
            public string Name { get; set; }
            public int MaxPlayers { get; set; }
            public int NumOfQuestionsInGame { get; set; }
            public int TimePerQuestion { get; set; }
            public int Status { get; set; }
        }

        public class GetRoomsResponse
        {
            public int Status { get; set; }
            public List<RoomData> Rooms { get; set; }
        }

        public class GetPlayersInRoomResponse
        {
            public List<string> Players { get; set; }
        }

        public class JoinRoomResponse
        {
            public int Status { get; set; }
        }

        public class CreateRoomResponse
        {
            public int Status { get; set; }
        }

        public class GetHighScoreResponse
        {
            public int Status { get; set; }
            public List<string> Statistics { get; set; }
        }

        public class GetPersonalStatsResponse
        {
            public int Status { get; set; }
            public List<string> Statistics { get; set; }
        }

    }
}

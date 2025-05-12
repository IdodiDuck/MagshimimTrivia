using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using TriviaClient.Models;

namespace TriviaClient.Constants
{
    internal class Responses
    {
        public class ErrorResponse
        {
            public string message { get; set; } = string.Empty;
        }

        // V1.0.0 Responses - 
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

        public class SignOutResponse
        {
            public uint status { get; set; }
        }

        public class CloseRoomResponse
        {
            public uint Status { get; set; }
        }

        public class StartGameResponse
        {
            public uint Status { get; set; }
        }

        public class GetRoomStateResponse
        {
            public uint Status { get; set; }
            public bool HasGameBegun { get; set; }
            public List<string> Players { get; set; } = new List<string>();
            public uint QuestionsCount { get; set; }
            public uint AnswerTimeout { get; set; }
        }

        public class LeaveRoomResponse
        {
            public uint Status { get; set; }
        }
    }

}

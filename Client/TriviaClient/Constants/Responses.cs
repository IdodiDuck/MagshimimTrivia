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

        // V2.0.0 Responses - 
        public class LogoutResponse
        {
            public uint status { get; set; }
        }

        public class GetRoomsResponse
        {
            public uint status { get; set; }
            public List<Room> rooms { get; set; } = new List<Room>();
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

        // V3.0.0 Responses - 
        public class CloseRoomResponse
        {
            public uint status { get; set; }
        }

        public class StartGameResponse
        {
            public uint status { get; set; }
        }

        public class GetRoomStateResponse
        {
            public uint status { get; set; }
            public bool hasGameBegun { get; set; }
            public List<string> players { get; set; } = new List<string>();
            public uint questionsCount { get; set; }
            public uint answerTimeout { get; set; }
        }

        public class LeaveRoomResponse
        {
            public uint status { get; set; }
        }

        // V4.0.0 Responses
        public class LeaveGameResponse
        {
            public uint Status { get; set; }
        }

        public class GetQuestionResponse
        {
            public uint Status { get; set; }
            public string Question { get; set; } = string.Empty;
            public Dictionary<uint, string> Answers { get; set; } = new Dictionary<uint, string>(); 
        }

        public class SubmitAnswerResponse
        {
            public uint Status { get; set; }
            public uint CorrectAnswerId { get; set; }
        }

        public class PlayerResults
        {
            public string Username { get; set; } = string.Empty;
            public uint CorrectAnswerCount { get; set; }
            public uint WrongAnswerCount { get; set; }
            public uint AverageAnswerTime { get; set; }
        }
    }

}

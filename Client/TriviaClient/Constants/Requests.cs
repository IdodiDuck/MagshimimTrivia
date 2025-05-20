using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TriviaClient.Constants
{
    public class LoginRequest
    {
        public string username { get; set; } = string.Empty;

        public string password { get; set; } = string.Empty;
    }

    public class SignupRequest
    {
        public string username { get; set; } = string.Empty;

        public string password { get; set; } = string.Empty;

        public string email { get; set; } = string.Empty;
    }

    public class GetPlayersInRoomRequest
    {
        public uint roomId { get; set; }
    }

    public class JoinRoomRequest
    {
        public uint roomId { get; set; }
    }

    public class CreateRoomRequest
    {
        public string roomName { get; set; } = string.Empty;

        public uint maxUsers { get; set; }

        public uint questionCount { get; set; }

        public uint answerTimeout { get; set; }
    }

}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TriviaClient.Constants
{
    public class LoginRequest
    {
        public string username { get; set; }

        public string password { get; set; }
    }

    public class SignupRequest
    {
        public string username { get; set; }

        public string password { get; set; }

        public string email { get; set; }
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
        public string roomName { get; set; }

        public uint maxUsers { get; set; }

        public uint questionCount { get; set; }

        public uint answerTimeout { get; set; }
    }
}

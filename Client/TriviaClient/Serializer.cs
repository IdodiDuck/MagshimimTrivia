using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using TriviaClient.Constants;

namespace TriviaClient
{
    internal static class Serializer
    {
        public static Byte[] SerializeRequest(LoginRequest loginRequest)
        {
            string json = JsonConvert.SerializeObject(loginRequest);

            return Encoding.ASCII.GetBytes(json);
        }

        public static Byte[] SerializeRequest(SignupRequest signupRequest) 
        {
            string json = JsonConvert.SerializeObject(signupRequest);

            return Encoding.ASCII.GetBytes(json);
        }

        public static Byte[] SerializeRequest(GetPlayersInRoomRequest getPlayersInRoomRequest)
        {
            string json = JsonConvert.SerializeObject(getPlayersInRoomRequest);

            return Encoding.ASCII.GetBytes(json);
        }

        public static Byte[] SerializeRequest(JoinRoomRequest joinRoomRequest)
        {
            string json = JsonConvert.SerializeObject(joinRoomRequest);

            return Encoding.ASCII.GetBytes(json);
        }
        
        public static Byte[] SerializeRequest(CreateRoomRequest createRoomRequest)
        {
            string json = JsonConvert.SerializeObject(createRoomRequest);

            return Encoding.ASCII.GetBytes(json);
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using TriviaClient.Constants;

namespace TriviaClient.Infrastructure
{
    internal static class Serializer
    {
        // V1 Serializations - 
        public static byte[] SerializeRequest(LoginRequest loginRequest)
        {
            const byte REQUEST_CODE = (byte)RequestCode.LoginRequest;
            string json = JsonConvert.SerializeObject(loginRequest);
            return BuildRequest(REQUEST_CODE, json);
        }

        public static byte[] SerializeRequest(SignupRequest signupRequest)
        {
            const byte REQUEST_CODE = (byte)RequestCode.SignupRequest;
            string json = JsonConvert.SerializeObject(signupRequest);
            return BuildRequest(REQUEST_CODE, json);
        }

        // V2 Serializations - 
        public static byte[] SerializeRequest(GetPlayersInRoomRequest getPlayersInRoomRequest)
        {
            const byte REQUEST_CODE = (byte)RequestCode.PlayersInRoomRequest;
            string json = JsonConvert.SerializeObject(getPlayersInRoomRequest);
            return BuildRequest(REQUEST_CODE, json);
        }

        public static byte[] SerializeRequest(JoinRoomRequest joinRoomRequest)
        {
            const byte REQUEST_CODE = (byte)RequestCode.JoinRoomRequest;
            string json = JsonConvert.SerializeObject(joinRoomRequest);
            return BuildRequest(REQUEST_CODE, json);
        }

        public static byte[] SerializeRequest(CreateRoomRequest createRoomRequest)
        {
            const byte REQUEST_CODE = (byte)RequestCode.CreateRoomRequest;
            string json = JsonConvert.SerializeObject(createRoomRequest);
            return BuildRequest(REQUEST_CODE, json);
        }

        // Support Methods - 
        private static byte[] BuildRequest(byte requestCode, string json)
        {
            const uint HEADERS_LENGTH = 5, CODE_BYTE_INDEX = 0, MESSAGE_LENGTH_INDEX = 1;
            const int JSON_SOURCE_BEGINNING = 0, JSON_DESTINATION_BEGINNING = 5;

            byte[] jsonBytes = Encoding.ASCII.GetBytes(json);
            byte[] fullRequest = new byte[HEADERS_LENGTH + jsonBytes.Length];

            fullRequest[CODE_BYTE_INDEX] = requestCode;

            byte[] jsonLengthBytes = BitConverter.GetBytes(jsonBytes.Length);
            if (BitConverter.IsLittleEndian)
            {
                Array.Reverse(jsonLengthBytes);
            }

            Array.Copy(jsonLengthBytes, 0, fullRequest, MESSAGE_LENGTH_INDEX, jsonLengthBytes.Length);
            Array.Copy(jsonBytes, JSON_SOURCE_BEGINNING, fullRequest, JSON_DESTINATION_BEGINNING, jsonBytes.Length);

            return fullRequest;
        }
    }
}

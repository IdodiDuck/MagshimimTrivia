using System;
using System.Collections.Generic;
using System.Data.SqlTypes;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using TriviaClient.Constants;
using static TriviaClient.Constants.Responses;

namespace TriviaClient
{
    internal class Deserializer
    {
        public static ErrorResponse? DeserializeErrorResponse(byte[] buffer)
        {
            try
            {
                string json = System.Text.Encoding.UTF8.GetString(buffer);
                return JsonConvert.DeserializeObject<ErrorResponse>(json);
            }
            catch
            {
                return null;
            }
        }

        public static LoginResponse? DeserializeLoginResponse(byte[] buffer)
        {
            try
            {
                string json = System.Text.Encoding.UTF8.GetString(buffer);
                return JsonConvert.DeserializeObject<LoginResponse>(json);
            }
            catch
            {
                return null;
            }
        }
        public static SignupResponse? DeserializeSignupResponse(byte[] buffer)
        {
            try
            {
                string json = System.Text.Encoding.UTF8.GetString(buffer);
                return JsonConvert.DeserializeObject<SignupResponse>(json);
            }
            catch
            {
                return null;
            }
        }
        public static LogoutResponse? DeserializeLogoutResponse(byte[] buffer)
        {
            try
            {
                string json = System.Text.Encoding.UTF8.GetString(buffer);
                return JsonConvert.DeserializeObject<LogoutResponse>(json);
            }
            catch
            {
                return null;
            }
        }
        public static GetRoomsResponse? DeserializeGetRoomsResponse(byte[] buffer)
        {
            try
            {
                string json = System.Text.Encoding.UTF8.GetString(buffer);
                return JsonConvert.DeserializeObject<GetRoomsResponse>(json);
            }
            catch
            {
                return null;
            }
        }
        public static GetPlayersInRoomResponse? DeserializeGetPlayersInRoomResponse(byte[] buffer)
        {
            try
            {
                string json = System.Text.Encoding.UTF8.GetString(buffer);
                return JsonConvert.DeserializeObject<GetPlayersInRoomResponse>(json);
            }
            catch
            {
                return null;
            }
        }
        public static JoinRoomResponse? DeserializeJoinRoomResponse(byte[] buffer)
        {
            try
            {
                string json = System.Text.Encoding.UTF8.GetString(buffer);
                return JsonConvert.DeserializeObject<JoinRoomResponse>(json);
            }
            catch
            {
                return null;
            }
        }
        public static CreateRoomResponse? DeserializeCreateRoomResponse(byte[] buffer)
        {
            try
            {
                string json = System.Text.Encoding.UTF8.GetString(buffer);
                return JsonConvert.DeserializeObject<CreateRoomResponse>(json);
            }
            catch
            {
                return null;
            }
        }
        public static GetHighScoreResponse? DeserializeGetHighScoreResponse(byte[] buffer)
        {
            try
            {
                string json = System.Text.Encoding.UTF8.GetString(buffer);
                return JsonConvert.DeserializeObject<GetHighScoreResponse>(json);
            }
            catch
            {
                return null;
            }
        }
        public static GetPersonalStatsResponse? DeserializeGetPersonalStatsResponse(byte[] buffer)
        {
            try
            {
                string json = System.Text.Encoding.UTF8.GetString(buffer);
                return JsonConvert.DeserializeObject<GetPersonalStatsResponse>(json);
            }
            catch
            {
                return null;
            }
        }

    }
}

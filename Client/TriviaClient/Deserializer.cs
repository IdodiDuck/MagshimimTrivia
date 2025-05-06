using System;
using System.Collections.Generic;
using System.Data.SqlTypes;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Newtonsoft.Json;
using TriviaClient.Constants;

namespace TriviaClient
{
    internal class Deserializer
    {
        public static T? DeserializeResponse<T>(byte[] data) where T : class
        {
            try
            {
                string json = System.Text.Encoding.UTF8.GetString(data);
                return JsonConvert.DeserializeObject<T>(json);
            }
            catch
            {
                return null;
            }
        }
    }
}

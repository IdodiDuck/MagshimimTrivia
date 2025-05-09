using System;
using System.Collections.Generic;
using System.Data.SqlTypes;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Text.Json;

namespace TriviaClient.Infrastructure
{
    internal class Deserializer
    {
        public static T? DeserializeResponse<T>(byte[] buffer)
        {
            string json = System.Text.Encoding.UTF8.GetString(buffer);

            return JsonSerializer.Deserialize<T>(json);
        }

    }
}

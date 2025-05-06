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
            const int HEADERS_BYTE_SIZE = 5;

            string json = System.Text.Encoding.ASCII.GetString(buffer[HEADERS_BYTE_SIZE..]);

            return JsonSerializer.Deserialize<T>(json);
        }

    }
}

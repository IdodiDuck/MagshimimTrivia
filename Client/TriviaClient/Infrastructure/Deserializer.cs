using System;
using System.Collections.Generic;
using System.Data.SqlTypes;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Text.Json;
using System.IO;
using System.Net.Sockets;

using TriviaClient.Constants;

namespace TriviaClient.Infrastructure
{
    internal class Deserializer
    {
        public static T? DeserializeResponse<T>(byte[] buffer)
        {
            const int EMPTY = 0;

            if (buffer == null || buffer.Length == EMPTY)
            {
                throw new IOException("Received empty buffer. The server may have disconnected.");
            }

            string json = System.Text.Encoding.UTF8.GetString(buffer[NetworkConstants.HEADERS_SIZE..]);

            return JsonSerializer.Deserialize<T>(json);
        }

    }
}

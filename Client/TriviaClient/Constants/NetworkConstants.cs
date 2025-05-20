using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TriviaClient.Constants
{
    public static class NetworkConstants
    {
        // General Networks Constants - 
        public const string SERVER_IP_ADDRESS = "127.0.0.1";
        public const int SERVER_PORT = 8820;

        public const int BUFFER_SIZE = 1024;

        // Protocol Constants - 
        public const int HEADERS_SIZE = 5;
        public const int CODE_INDEX = 0;

    }
}

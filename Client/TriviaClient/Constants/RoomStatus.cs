using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TriviaClient.Constants
{
    public enum RoomStatus : uint
    {
        OPENED = 0,
        CLOSED,
        GAME_STARTED
    }
}

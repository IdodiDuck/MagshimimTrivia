using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TriviaClient.Constants
{
    public enum GameState : uint
    {
        WAITING_TO_START = 0,
        STARTED,
        WAITING_FOR_NEXT_QUESTION,
        WAITING_FOR_ANSWER,
        FINISHED
    }
}

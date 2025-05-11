using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TriviaClient.Models
{
    public class RoomData
    {
        public uint id { get; set; }
        public string name { get; set; } = string.Empty;
        public uint maxPlayers { get; set; }
        public uint numOfQuestionsInGame { get; set; }
        public uint timePerQuestion { get; set; }
        public uint status { get; set; }
    }
}

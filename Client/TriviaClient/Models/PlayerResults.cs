using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TriviaClient.Models
{
    public class PlayerResults
    {
        public string username { get; set; } = string.Empty;
        public uint correctAnswerCount { get; set; }
        public uint wrongAnswerCount { get; set; }
        public uint averageAnswerTime { get; set; }
    }
}

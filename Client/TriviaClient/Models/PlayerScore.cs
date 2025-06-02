using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TriviaClient.Models
{
    public class PlayerScore
    {
        public string Name { get; set; } = string.Empty;
        public int CorrectAnswers { get; set; }
        public string AvgTime { get; set; } = string.Empty;
    }
}

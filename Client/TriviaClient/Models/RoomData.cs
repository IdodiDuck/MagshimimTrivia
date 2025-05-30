using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using TriviaClient.Constants;

namespace TriviaClient.Models
{
    public class RoomData
    {
        // Attributes - 
        public uint id { get; set; }
        public string name { get; set; } = string.Empty;
        public uint maxPlayers { get; set; }
        public uint numOfQuestionsInGame { get; set; }
        public uint timePerQuestion { get; set; }
        public RoomStatus status { get; set; }

        public string adminName { get; set; } = string.Empty;

        public uint numberOfMembers { get; set; }

        // C'tor - 
        public RoomData(uint id, string name, uint maxPlayers, uint numOfQuestionsInGame, uint timePerQuestion, RoomStatus status, string adminName, uint numberOfMembers)
        {
            this.id = id;
            this.name = name;
            this.maxPlayers = maxPlayers;
            this.numOfQuestionsInGame = numOfQuestionsInGame;
            this.timePerQuestion = timePerQuestion;
            this.status = status;
            this.adminName = adminName;
            this.numberOfMembers = numberOfMembers;
        }

        // String Representation Method - 
        public override string ToString()
        {
            string statusStr = (status == RoomStatus.GAME_STARTED) ? ", Game Started!" : "";
            return $"{name}{statusStr}, Admin: {adminName}";
        }  

    }
}

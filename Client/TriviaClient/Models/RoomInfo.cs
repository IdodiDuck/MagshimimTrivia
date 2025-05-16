using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using TriviaClient.Constants;


namespace TriviaClient.Models
{
    public class RoomInfo
    {
        // Attributes - 
        public string RoomName { get; set; }
        public string AdminName { get; set; }
        public uint NumberOfMembers { get; set; }
        public uint roomId { get; set; }

        // C'tor - 
        public RoomInfo(string roomName, string adminName, uint numberOfPeople, uint roomid, RoomStatus roomStatus)
        {
            if (roomStatus == RoomStatus.GAME_STARTED)
            {
                this.RoomName = roomName + ", Game Started!";
            }

            else
            {
                this.RoomName = roomName;
            }

            AdminName = adminName;
            NumberOfMembers = numberOfPeople;
            this.roomId = roomid;
        }

        // Object String Representation - 
        public override string ToString()
        {
            return this.RoomName + ", Admin: " + this.AdminName + ", Current in Room: " + this.NumberOfMembers;
        }
    }
}

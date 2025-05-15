using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Navigation;
using TriviaClient.Constants;
using TriviaClient.Infrastructure;

namespace TriviaClient
{
    /// <summary>
    /// Interaction logic for GameLobby.xaml
    /// </summary>
    /// 


    public partial class GameLobby : Page
    {
        private readonly Communicator m_communicator;

        private string RoomName { get; set; } = string.Empty;
        private uint MaxPlayer { get; set; }
        private uint QuestionAmount { get; set; }
        private uint TimePerQuestion { get; set; }

        public GameLobby(Communicator communicator, string roomName, int maxPlayer, int questionAmount, int timePerQuestion)
        {
            InitializeComponent();

            m_communicator = communicator;

            RoomName = roomName;
            MaxPlayer = (uint)maxPlayer;
            QuestionAmount = (uint)questionAmount;
            TimePerQuestion = (uint)timePerQuestion;

            RoomNameText.Text = roomName;
            MaxPlayersText.Text = maxPlayer.ToString();
            QuestionCountText.Text = questionAmount.ToString();
            TimePerQuestionText.Text = timePerQuestion.ToString();
        }

        private void StartGameButton_Click(object sender, RoutedEventArgs e)
        {
            // TODO AT V4.0.0
        }

        private void LeaveButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var leaveRoomRequest = Serializer.SerializeEmptyRequest(RequestCode.LEAVE_ROOM_REQUEST);
                m_communicator.SendToServer(leaveRoomRequest);
            }

            catch (Exception ex)
            {
                MessageBox.Show($"Error while leaving room: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }

            if (NavigationService.CanGoBack)
            {
                NavigationService.GoBack();
                return;
            }

            MessageBox.Show("Error: There's no previous page you can go back to!");
        }
    }

}

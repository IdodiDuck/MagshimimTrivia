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

using static TriviaClient.Constants.Responses;
using System.Windows.Navigation;
using TriviaClient.Constants;
using TriviaClient.Infrastructure;
using System.IO;
using System.Runtime.Serialization;

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

        public GameLobby(Communicator communicator, string roomName, uint maxPlayer, uint questionAmount, uint timePerQuestion, bool isAdmin)
        {
            InitializeComponent();

            this.m_communicator = communicator;

            this.RoomName = roomName;
            this.MaxPlayer = maxPlayer;
            this.QuestionAmount = questionAmount;
            this.TimePerQuestion = timePerQuestion;

            RoomNameText.Text = roomName;
            MaxPlayersText.Text = maxPlayer.ToString();
            QuestionCountText.Text = questionAmount.ToString();
            TimePerQuestionText.Text = timePerQuestion.ToString();

            if (isAdmin)
            {
                AdminButtonsPanel.Visibility = Visibility.Visible;
            }
            
            else
            {
                LeaveButton.Visibility = Visibility.Visible;
            }
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
                var serverResponse = Deserializer.DeserializeResponse<LeaveRoomResponse>(m_communicator.ReceiveFromServer());

                if (serverResponse == null)
                {
                    return;
                }

                if (serverResponse.Status == StatusCodes.SUCCESS)
                {
                    if (NavigationService.CanGoBack)
                    {
                        NavigationService.GoBack();
                        NavigationService.GoBack();
                        return;
                    }

                    MessageBox.Show("Error: There's no previous page you can go back to!", "Navigation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                    return;
                }

                MessageBox.Show("Failed to leave room. Server returned an error.", "Leave Room", MessageBoxButton.OK, MessageBoxImage.Warning);
            }

            catch (IOException ex)
            {
                MessageBox.Show($"Connection Error: {ex.Message}", "Network Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }

            catch (SerializationException ex)
            {
                MessageBox.Show($"Data serialization error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }

            catch (Exception ex)
            {
                MessageBox.Show($"An unexpected error occurred: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void CloseRoomButton_Click(object sender, RoutedEventArgs e)
        {
            m_communicator.SendToServer(Serializer.SerializeEmptyRequest(RequestCode.CLOSE_ROOM_REQUEST));
            var serverResponse = Deserializer.DeserializeResponse<CloseRoomResponse>(m_communicator.ReceiveFromServer());

            if (serverResponse == null)
            {
                return;
            }

            if (serverResponse.Status == StatusCodes.SUCCESS)
            {
                // Stop Thread which is checking Room State!!
                if (NavigationService.CanGoBack)
                {
                    NavigationService.GoBack();
                    NavigationService.GoBack();
                    return;
                }
            }

            MessageBox.Show("Failed to Close room", "Close room", MessageBoxButton.OK, MessageBoxImage.Information);
        }
    }

}

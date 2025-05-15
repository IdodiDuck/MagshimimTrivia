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
using static TriviaClient.Constants.Responses;
using TriviaClient.Infrastructure;

namespace TriviaClient
{
    /// <summary>
    /// Interaction logic for CreateRoom.xaml
    /// </summary>
    public partial class CreateRoom : Page
    {
        public CreateRoom()
        {
            InitializeComponent();
        }

        private void CreateButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (!int.TryParse(PlayersNumberTextBox.Text, out int maxUsers) || maxUsers <= 0)
                {
                    MessageBox.Show("Please enter a valid number of players", "Invalid Input");
                    return;
                }

                if (!int.TryParse(QuestionsNumberTextBox.Text, out int questionCount) || questionCount <= 0)
                {
                    MessageBox.Show("Please enter a valid number of questions", "Invalid Input");
                    return;
                }

                if (!int.TryParse(QuestionTimeTextBox.Text, out int answerTimeout) || answerTimeout <= 0)
                {
                    MessageBox.Show("Please enter a valid time per question", "Invalid Input");
                    return;
                }

                var request = new CreateRoomRequest()
                {
                    roomName = RoomNameTextBox.Text,
                    maxUsers = (uint)maxUsers,
                    questionCount = (uint)questionCount,
                    answerTimeout = (uint)answerTimeout
                };

                byte[] serializedRequest = Serializer.SerializeRequest(request);
                Globals.Communicator.SendToServer(serializedRequest);
                byte[] response = Globals.Communicator.ReceiveFromServer();
                var createRoomResponse = Deserializer.DeserializeResponse<CreateRoomResponse>(response);

                if (createRoomResponse?.status == StatusCodes.SUCCESS)
                {
                    RoomNameTextBox.Clear();
                    PlayersNumberTextBox.Clear();
                    QuestionsNumberTextBox.Clear();
                    QuestionTimeTextBox.Clear();

                    this.NavigationService.Navigate(new GameLobby(request.roomName, maxUsers, questionCount, answerTimeout));
                }

                else
                {
                    MessageBox.Show("Failed to create room", "Error");
                }
            }

            catch (Exception ex)
            {
                MessageBox.Show($"Error creating room: {ex.Message}", "Error");
            }
        }

        private void BackButton_Click(object sender, RoutedEventArgs e)
        {
            if (NavigationService.CanGoBack)
            {
                NavigationService.GoBack();
                return;
            }

           MessageBox.Show("Error: There's no previous page you can go back to!");
        }

        private void Input_TextChanged(object sender, TextChangedEventArgs e)
        {
            CreateButton.IsEnabled = !string.IsNullOrWhiteSpace(RoomNameTextBox.Text) &&
                                   !string.IsNullOrWhiteSpace(PlayersNumberTextBox.Text) &&
                                   !string.IsNullOrWhiteSpace(QuestionsNumberTextBox.Text) &&
                                   !string.IsNullOrWhiteSpace(QuestionTimeTextBox.Text);
        }
    }


}

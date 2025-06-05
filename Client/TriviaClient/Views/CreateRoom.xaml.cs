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
using System.IO;
using System.Runtime.Serialization;

namespace TriviaClient
{
    /// <summary>
    /// Interaction logic for CreateRoom.xaml
    /// </summary>
    public partial class CreateRoom : Page
    {
        private string m_username { get; set; } = string.Empty;
        private readonly Communicator m_communicator;

        public CreateRoom(Communicator communicator, string username)
        {
            InitializeComponent();
            m_communicator = communicator;
            m_username = username;
        }

        private void CreateButton_Click(object sender, RoutedEventArgs e)
        {
            const bool IS_ADMIN = true;
            const int EMPTY = 0, MIN_PLAYERS = 2;

            try
            {
                if (!int.TryParse(PlayersNumberTextBox.Text, out int maxUsers) || maxUsers < MIN_PLAYERS)
                {
                    ShowError("Please enter a valid number of players");
                    return;
                }

                if (!int.TryParse(QuestionsNumberTextBox.Text, out int questionCount) || questionCount <= EMPTY)
                {
                    ShowError("Please enter a valid number of questions");
                    return;
                }

                if (!int.TryParse(QuestionTimeTextBox.Text, out int answerTimeout) || answerTimeout <= EMPTY)
                {
                    ShowError("Please enter a valid time per question");
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
                m_communicator.SendToServer(serializedRequest);
                byte[] response = m_communicator.ReceiveFromServer();
                var createRoomResponse = Deserializer.DeserializeResponse<CreateRoomResponse>(response);

                if (createRoomResponse?.status == StatusCodes.SUCCESS)
                {
                    RoomNameTextBox.Clear();
                    PlayersNumberTextBox.Clear();
                    QuestionsNumberTextBox.Clear();
                    QuestionTimeTextBox.Clear();

                    this.NavigationService.Navigate(new GameLobby(m_communicator, request.roomName, (uint)maxUsers, (uint)questionCount, (uint)answerTimeout, IS_ADMIN, m_username));
                }

                else
                {
                    ShowError("Error:  Failed to create room");
                }
            }

            catch (IOException ex)
            {
                ShowError($"Connection Error: {ex.Message}");
            }

            catch (SerializationException ex)
            {
                ShowError($"Data serialization error: {ex.Message}");
            }

            catch (Exception ex)
            {
                ShowError($"An unexpected error occurred: {ex.Message}");
            }
        }

        private void BackButton_Click(object sender, RoutedEventArgs e)
        {
            if (NavigationService.CanGoBack)
            {
                NavigationService.GoBack();
                return;
            }

            ShowError("Error: There's no previous page you can go back to!");
        }

        private void Input_TextChanged(object sender, TextChangedEventArgs e)
        {
            CreateButton.IsEnabled = !string.IsNullOrWhiteSpace(RoomNameTextBox.Text) &&
                                     !string.IsNullOrWhiteSpace(PlayersNumberTextBox.Text) &&
                                     !string.IsNullOrWhiteSpace(QuestionsNumberTextBox.Text) &&
                                     !string.IsNullOrWhiteSpace(QuestionTimeTextBox.Text);
        }

        private void ShowError(string message)
        {
            ErrorTextBlock.Text = message;
            ErrorTextBlock.Visibility = Visibility.Visible;
        }
    }
}

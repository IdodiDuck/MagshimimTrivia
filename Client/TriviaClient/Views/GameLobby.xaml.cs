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
    public partial class GameLobby : Page
    {
        private readonly Communicator m_communicator;
        private Thread? m_roomStatusThread;
        private bool m_checkRoomStatus;
        private bool m_isAdmin;

        private string RoomName { get; set; } = string.Empty;
        private uint MaxPlayer { get; set; }
        private uint QuestionAmount { get; set; }
        private uint TimePerQuestion { get; set; }

        public GameLobby(Communicator communicator, string roomName, uint maxPlayer, uint questionAmount, uint timePerQuestion, bool isAdmin)
        {
            InitializeComponent();

            this.m_communicator = communicator;
            this.m_isAdmin = isAdmin;

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

            this.Loaded += CheckStatus_Loaded;
            this.Unloaded += CheckStatus_Unloaded;

            if (this.NavigationService != null)
            {
                this.NavigationService.Navigating += NavigationService_Navigating;
            }
        }

        private void CheckStatus_Loaded(object sender, RoutedEventArgs e)
        {
            m_checkRoomStatus = true;
            m_roomStatusThread = new Thread(CheckRoomStatus);
            m_roomStatusThread.Start();
        }

        private void StartRoomStatusThread()
        {
            if (m_roomStatusThread == null || !m_roomStatusThread.IsAlive)
            {
                m_roomStatusThread = new Thread(CheckRoomStatus);
                m_roomStatusThread.Start();
            }
        }

        private void CheckStatus_Unloaded(object sender, RoutedEventArgs e)
        {
            m_checkRoomStatus = false;
            if (m_roomStatusThread?.IsAlive == false)
            {
                m_roomStatusThread.Join();
            }
        }

        private void CheckRoomStatus()
        {
            const int THREE_SECONDS = 3000;

            while (m_checkRoomStatus)
            {
                try
                {
                    var getRoomStateRequest = Serializer.SerializeEmptyRequest(RequestCode.GET_ROOM_STATE_REQUEST);
                    m_communicator.SendToServer(getRoomStateRequest);
                    var serverResponse = Deserializer.DeserializeResponse<GetRoomStateResponse>(m_communicator.ReceiveFromServer());

                    if (serverResponse == null || serverResponse.Status != StatusCodes.SUCCESS)
                    {
                        MessageBox.Show("Failed to check room status.");
                        return;
                    }

                    Dispatcher.Invoke(() =>
                    {
                        if (serverResponse.HasGameBegun)
                        {
                            m_checkRoomStatus = false;
                            MessageBox.Show("The game has started!", "Game Started", MessageBoxButton.OK, MessageBoxImage.Information);
                            // NavigationService.Navigate(new GamePage(m_communicator));
                        }

                        if (serverResponse.Status != StatusCodes.SUCCESS)
                        {
                            m_checkRoomStatus = false;
                            MessageBox.Show("The room has been closed by the admin.", "Room Closed", MessageBoxButton.OK, MessageBoxImage.Information);
                            NavigationService.GoBack();
                        }
                    });

                    Thread.Sleep(THREE_SECONDS);
                }
                catch (IOException ex)
                {
                    Dispatcher.Invoke(() =>
                    {
                        m_checkRoomStatus = false;
                        MessageBox.Show($"Connection Error: {ex.Message}", "Network Error", MessageBoxButton.OK, MessageBoxImage.Error);
                        NavigationService.GoBack();
                    });
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Error in room status thread: {ex.Message}");
                    Thread.Sleep(THREE_SECONDS);
                }
            }
        }

        private void NavigationService_Navigating(object sender, NavigatingCancelEventArgs e)
        {
            if (e.Content == this)
            {
                StartRoomStatusThread();
            }
        }

        private void StartGameButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var startGameRequest = Serializer.SerializeEmptyRequest(RequestCode.START_GAME_REQUEST);
                m_communicator.SendToServer(startGameRequest);
                var serverResponse = Deserializer.DeserializeResponse<StartGameResponse>(m_communicator.ReceiveFromServer());

                if (serverResponse?.Status == StatusCodes.SUCCESS)
                {
                    MessageBox.Show("Game started successfully!", "Success", MessageBoxButton.OK, MessageBoxImage.Information);
                    // NavigationService.Navigate(new GamePage(m_communicator));
                }
                else
                {
                    MessageBox.Show("Failed to start the game.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error starting game: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
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
            catch (Exception ex)
            {
                MessageBox.Show($"An unexpected error occurred: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void CloseRoomButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                m_communicator.SendToServer(Serializer.SerializeEmptyRequest(RequestCode.CLOSE_ROOM_REQUEST));
                var serverResponse = Deserializer.DeserializeResponse<CloseRoomResponse>(m_communicator.ReceiveFromServer());

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
                }

                MessageBox.Show("Failed to Close room", "Close room", MessageBoxButton.OK, MessageBoxImage.Information);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error closing room: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }
    }

}

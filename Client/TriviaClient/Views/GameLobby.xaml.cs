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
        private Thread? m_roomStatesThread;
        private bool m_refreshPage;
        private bool m_isAdmin;

        private string RoomName { get; set; } = string.Empty;
        private uint MaxPlayer { get; set; }
        private uint QuestionAmount { get; set; }
        private uint TimePerQuestion { get; set; }

        public GameLobby(Communicator communicator, string roomName, uint maxPlayer, uint questionAmount, uint timePerQuestion, bool isAdmin)
        {
            InitializeComponent();

            m_communicator = communicator;

            this.RoomName = roomName;
            this.MaxPlayer = maxPlayer;
            this.QuestionAmount = questionAmount;
            this.TimePerQuestion = timePerQuestion;

            RoomNameText.Text = roomName;
            MaxPlayersText.Text = maxPlayer.ToString();
            QuestionCountText.Text = questionAmount.ToString();
            TimePerQuestionText.Text = timePerQuestion.ToString();

            AdminButtonsPanel.Visibility = isAdmin ? Visibility.Visible : Visibility.Collapsed;
            LeaveButton.Visibility = isAdmin ? Visibility.Collapsed : Visibility.Visible;

            Loaded += GameLobby_Loaded;
            Unloaded += GameLobby_Unloaded;

            if (NavigationService != null)
            {
                NavigationService.Navigating += NavigationService_Navigating;
            }
        }

        private void GameLobby_Loaded(object sender, RoutedEventArgs e)
        {
            m_refreshPage = true;
            m_roomStatesThread = new Thread(refreshRoomStates);
            m_roomStatesThread.Start();
        }

        private void GameLobby_Unloaded(object sender, RoutedEventArgs e)
        {
            m_refreshPage = false;

            if (m_roomStatesThread != null && m_roomStatesThread.IsAlive)
            {
                m_roomStatesThread.Join();
            }
        }

        private void refreshRoomStates()
        {
            const int THREE_SECONDS = 3000;

            while (m_refreshPage && m_communicator.IsConnected)
            {
                try
                {
                    var request = Serializer.SerializeEmptyRequest(RequestCode.GET_ROOM_STATE_REQUEST);
                    m_communicator.SendToServer(request);
                    var response = Deserializer.DeserializeResponse<GetRoomStateResponse>(m_communicator.ReceiveFromServer());

                    if (response == null)
                    {
                        SafeInvoke(() =>
                        {
                            MessageBox.Show("Invalid room state response.", "Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                            NavigationService?.GoBack();
                        });
                        return;
                    }

                    SafeInvoke(() =>
                    {
                        PlayersListView.ItemsSource = null;
                        PlayersListView.ItemsSource = response.players;

                        if (response.hasGameBegun)
                        {
                            m_refreshPage = false;
                            MessageBox.Show("The game has started!", "Game Started", MessageBoxButton.OK, MessageBoxImage.Information);
                            // NavigationService.Navigate(new GamePage(m_communicator));
                        }

                        else if (response.status != StatusCodes.SUCCESS)
                        {
                            m_refreshPage = false;
                            MessageBox.Show("The room has been closed by the admin.", "Room Closed", MessageBoxButton.OK, MessageBoxImage.Information);
                            NavigationService?.GoBack();
                        }
                    });

                    Thread.Sleep(THREE_SECONDS);
                }

                catch (IOException ex)
                {
                    Dispatcher.Invoke(() =>
                    {
                        m_refreshPage = false;
                        MessageBox.Show($"Connection Error: {ex.Message}", "Network Error", MessageBoxButton.OK, MessageBoxImage.Error);
                        NavigationService?.GoBack();
                    });
                }

                catch (TaskCanceledException)
                {
                    m_refreshPage = false;
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
                m_refreshPage = true;
                m_roomStatesThread = new Thread(refreshRoomStates);
                m_roomStatesThread.Start();
            }
        }

        private void StartGameButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var startGameRequest = Serializer.SerializeEmptyRequest(RequestCode.START_GAME_REQUEST);
                m_communicator.SendToServer(startGameRequest);
                var serverResponse = Deserializer.DeserializeResponse<StartGameResponse>(m_communicator.ReceiveFromServer());

                if (serverResponse?.status == StatusCodes.SUCCESS)
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

                if (serverResponse?.status == StatusCodes.SUCCESS)
                {
                    if (NavigationService.CanGoBack)
                    {
                        NavigationService.GoBack();
                        NavigationService.GoBack();
                    }

                    else
                    {
                        MessageBox.Show("There's no previous page to return to.", "Navigation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                    }
                }

                else
                {
                    MessageBox.Show("Failed to leave room.", "Leave Room", MessageBoxButton.OK, MessageBoxImage.Warning);
                }
            }

            catch (IOException ex)
            {
                MessageBox.Show($"Connection Error: {ex.Message}", "Network Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }

            catch (Exception ex)
            {
                MessageBox.Show($"Unexpected error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void CloseRoomButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                m_communicator.SendToServer(Serializer.SerializeEmptyRequest(RequestCode.CLOSE_ROOM_REQUEST));
                var serverResponse = Deserializer.DeserializeResponse<CloseRoomResponse>(m_communicator.ReceiveFromServer());

                if (serverResponse?.status == StatusCodes.SUCCESS)
                {
                    m_refreshPage = false;

                    if ((m_roomStatesThread != null) && (m_roomStatesThread.IsAlive))
                    {
                        m_roomStatesThread.Join();
                    }

                    if (NavigationService.CanGoBack)
                    {
                        NavigationService.GoBack();
                        NavigationService.GoBack();
                    }
                }

                else
                {
                    MessageBox.Show("Failed to close the room.", "Close Room", MessageBoxButton.OK, MessageBoxImage.Warning);
                }
            }

            catch (Exception ex)
            {
                MessageBox.Show($"Error closing room: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void SafeInvoke(Action action)
        {
            try
            {
                if (Dispatcher != null && !Dispatcher.HasShutdownStarted && !Dispatcher.HasShutdownFinished)
                {
                    Dispatcher.Invoke(action);
                }
            }

            catch (TaskCanceledException)
            {

            }

            catch (Exception ex)
            {
                Console.WriteLine($"Dispatcher.Invoke error: {ex.Message}");
            }
        }
    }


}

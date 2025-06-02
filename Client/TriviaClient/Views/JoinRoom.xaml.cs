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

using static TriviaClient.Constants.Responses;
using TriviaClient.Constants;
using TriviaClient.Infrastructure;
using TriviaClient.Models;
using System.IO;
using System.Runtime.Serialization;

namespace TriviaClient
{
    /// <summary>
    /// Interaction logic for JoinRoom.xaml
    /// </summary>
    public partial class JoinRoom : Page
    {
        private string m_username { get; set; } = string.Empty;
        private readonly Communicator m_communicator;
        private Thread? m_refreshRoomsThread;
        private bool m_refreshPage;

        public JoinRoom(Communicator communicator)
        {
            InitializeComponent();
            m_communicator = communicator;
            m_refreshPage = true;

            this.Loaded += Room_Loaded;
            this.Unloaded += JoinRoom_Unloaded;

            if (this.NavigationService != null)
            {
                this.NavigationService.Navigating += NavigationService_Navigating;
            }
        }

        private void Room_Loaded(object sender, RoutedEventArgs e)
        {
            m_refreshPage = true;
            m_refreshRoomsThread = new Thread(refreshPage);
            m_refreshRoomsThread.Start();
        }

        private void JoinRoom_Unloaded(object sender, RoutedEventArgs e)
        {
            m_refreshPage = false;
            if (m_refreshRoomsThread != null && m_refreshRoomsThread.IsAlive)
            {
                m_refreshRoomsThread.Join();
            }
        }

        private void NavigationService_Navigating(object sender, NavigatingCancelEventArgs e)
        {
            if (e.Content == this)
            {
                StartRefreshingRooms();
            }
        }

        private void StartRefreshingRooms()
        {
            m_refreshPage = true;

            if (m_refreshRoomsThread == null || !m_refreshRoomsThread.IsAlive)
            {
                m_refreshRoomsThread = new Thread(refreshPage);
                m_refreshRoomsThread.Start();
            }
        }

        private void JoinButton_Click(object sender, RoutedEventArgs e)
        {
            const bool IS_NOT_ADMIN = false;

            try
            {
                if (RoomsList.SelectedItem is not RoomData selectedRoomInfo)
                {
                    MessageBox.Show("Please select a room to join.");
                    return;
                }

                var joinRequest = new JoinRoomRequest { roomId = selectedRoomInfo.id };
                m_communicator.SendToServer(Serializer.SerializeRequest(joinRequest));

                byte[] responseBytes = m_communicator.ReceiveFromServer();
                var joinResponse = Deserializer.DeserializeResponse<JoinRoomResponse>(responseBytes);

                if (joinResponse == null)
                {
                    MessageBox.Show("Invalid response from server.", "Server Error",
                                  MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }

                if (responseBytes[NetworkConstants.CODE_INDEX] == (byte)(ResponseCode.ERROR_RESPONSE))
                {
                    ErrorResponse? errorResponse = Deserializer.DeserializeResponse<ErrorResponse>(responseBytes);
                    MessageBox.Show(errorResponse?.message, "Error", MessageBoxButton.OK, MessageBoxImage.Information);
                    return;
                }

                if (joinResponse.status == StatusCodes.SUCCESS)
                {
                    m_refreshPage = false;
                    this.NavigationService.Navigate(new GameLobby(m_communicator, selectedRoomInfo.name, selectedRoomInfo.maxPlayers,
                        selectedRoomInfo.numOfQuestionsInGame, selectedRoomInfo.timePerQuestion, IS_NOT_ADMIN, m_username));
                    return;
                    
                }

                MessageBox.Show("Failed to join the selected room.");
                return;
            }

            catch (TaskCanceledException)
            {

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


        private void BackButton_Click(object sender, RoutedEventArgs e)
        {
            m_refreshPage = false;

            if ((m_refreshRoomsThread != null) && (m_refreshRoomsThread.IsAlive))
            {
                m_refreshRoomsThread.Join();
            }

            if (NavigationService.CanGoBack)
            {
                NavigationService.GoBack();
                return;
            }

            MessageBox.Show("Error: There's no previous page you can go back to!");
        }

        private void RoomsList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            JoinRoomBtn.IsEnabled = RoomsList.SelectedItem != null;

            if (RoomsList.SelectedItem is RoomData selectedRoom)
            {
                LoadPlayersForRoom(selectedRoom.id);
                return;
            }

            UsersList.ItemsSource = null;
        }

        private void refreshPage()
        {
            const int THREE_SECONDS = 3000;

            while (m_refreshPage && m_communicator.IsConnected)
            {
                try
                {
                    List<RoomData> roomsList = GetAvailableRooms();

                    if ((this.Dispatcher.HasShutdownStarted) || (this.Dispatcher.HasShutdownFinished))
                    {
                        break;
                    }

                    UpdateRoomsListUI(roomsList);
                }

                catch
                {
                    break;
                }

                Thread.Sleep(THREE_SECONDS);
            }

        }

        // Support Methods - 
        private List<RoomData> GetAvailableRooms()
        {
            const int EMPTY = 0, ADMIN_INDEX = 0;
            List<RoomData> roomsList = new List<RoomData>();

            m_communicator.SendToServer(Serializer.SerializeEmptyRequest(RequestCode.RoomsRequest));
            var response = Deserializer.DeserializeResponse<GetRoomsResponse>(m_communicator.ReceiveFromServer());

            if (response != null && response.status == StatusCodes.SUCCESS)
            {
                foreach (var currentRoom in response.rooms)
                {
                    var getPlayersRequest = new GetPlayersInRoomRequest { roomId = currentRoom.id };
                    m_communicator.SendToServer(Serializer.SerializeRequest(getPlayersRequest));
                    var getPlayersResponse = Deserializer.DeserializeResponse<GetPlayersInRoomResponse>(m_communicator.ReceiveFromServer());

                    if (getPlayersResponse == null || getPlayersResponse.players.Count == EMPTY)
                    {
                        continue;
                    }

                    roomsList.Add(new RoomData(currentRoom.id, currentRoom.name, currentRoom.maxPlayers, currentRoom.numOfQuestionsInGame,
                        currentRoom.timePerQuestion, currentRoom.status, getPlayersResponse.players[ADMIN_INDEX], (uint)getPlayersResponse.players.Count()));
                }
            }

            return roomsList;
        }

        private void UpdateRoomsListUI(List<RoomData> roomsList)
        {
            const int EMPTY = 0;

            this.SafeInvoke(() =>
            {
                var selectedRoom = RoomsList.SelectedItem as RoomData;
                uint? selectedRoomId = selectedRoom?.id;

                RoomsList.ItemsSource = null;
                RoomsList.ItemsSource = roomsList;

                // Restore selection if still exists
                if (selectedRoomId.HasValue)
                {
                    var itemToSelect = roomsList.FirstOrDefault(desiredRoom => desiredRoom.id == selectedRoomId.Value);
                    if (itemToSelect != null)
                    {
                        RoomsList.SelectedItem = itemToSelect;
                    }
                }

                NoRoomsText.Visibility = (roomsList.Count == EMPTY) ? Visibility.Visible : Visibility.Collapsed;
                JoinRoomBtn.IsEnabled = RoomsList.SelectedItem != null;
            });
        }

        private void LoadPlayersForRoom(uint roomId)
        {
            try
            {
                var request = new GetPlayersInRoomRequest { roomId = roomId };
                m_communicator.SendToServer(Serializer.SerializeRequest(request));

                var response = Deserializer.DeserializeResponse<GetPlayersInRoomResponse>(m_communicator.ReceiveFromServer());

                if (response?.players != null)
                {
                    UsersList.ItemsSource = response.players;
                    return;
                }

                UsersList.ItemsSource = null;

            }

            catch (Exception ex)
            {
                MessageBox.Show($"Error retrieving players list: {ex.Message}");
                UsersList.ItemsSource = null;

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

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

namespace TriviaClient
{
    /// <summary>
    /// Interaction logic for JoinRoom.xaml
    /// </summary>
    public partial class JoinRoom : Page
    {
        private readonly Communicator m_communicator;
        private Thread? m_refreshRoomsThread;
        private bool m_refreshPage;

        public JoinRoom(Communicator communicator)
        {
            InitializeComponent();
            m_communicator = communicator;
            m_refreshPage = true;

            this.Loaded += Room_Loaded;
        }

        private void Room_Loaded(object sender, RoutedEventArgs e)
        {
            m_refreshPage = true;
            m_refreshRoomsThread = new Thread(refreshPage);
            m_refreshRoomsThread.Start();
        }

        private void JoinButton_Click(object sender, RoutedEventArgs e)
        {
            const int EMPTY = 0;

            //TODO FIX METHOD THIS IF WE HAVE SELECTED A ROOM ALREADY
            try
            {
                var request = Serializer.SerializeEmptyRequest(RequestCode.RoomsRequest);
                m_communicator.SendToServer(request);
                byte[] serverResponse = m_communicator.ReceiveFromServer();
                var response = Deserializer.DeserializeResponse<GetRoomsResponse>(serverResponse);

                if (response == null || response.status != StatusCodes.SUCCESS)
                {
                    MessageBox.Show("Failed to fetch rooms.");
                    return;
                }

                if (response.rooms.Count == EMPTY)
                {
                    MessageBox.Show("No rooms available. Please create a new room.");
                    return;
                }

                RoomData selectedRoom = response.rooms[0];

                this.NavigationService.Navigate(
                    new GameLobby(
                        m_communicator,
                        selectedRoom.name,
                        (int)selectedRoom.maxPlayers,
                        (int)selectedRoom.numOfQuestionsInGame,
                        (int)selectedRoom.timePerQuestion
                    ));
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error joining room: {ex.Message}");
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
        }

        private void refreshPage()
        {
            const int HALF_SECOND = 500;

            while (m_refreshPage)
            {
                try
                {
                    List<RoomInfo> roomsList = GetAvailableRooms();

                    if (this.Dispatcher.HasShutdownStarted || this.Dispatcher.HasShutdownFinished)
                    {
                        break;
                    }

                    UpdateRoomsListUI(roomsList);
                }

                catch
                {
                    break;
                }

                Thread.Sleep(HALF_SECOND);
            }

        }

        // Support Methods - 
        private List<RoomInfo> GetAvailableRooms()
        {
            const int EMPTY = 0, ADMIN_INDEX = 0;
            List<RoomInfo> roomsList = new();

            var request = Serializer.SerializeEmptyRequest(RequestCode.RoomsRequest);
            m_communicator.SendToServer(request);
            byte[] responseBytes = m_communicator.ReceiveFromServer();
            var response = Deserializer.DeserializeResponse<GetRoomsResponse>(responseBytes);

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

                    roomsList.Add(new RoomInfo(
                        currentRoom.name,
                        getPlayersResponse.players[ADMIN_INDEX],
                        (uint)getPlayersResponse.players.Count,
                        currentRoom.id,
                        currentRoom.status));
                }
            }

            return roomsList;
        }

        private void UpdateRoomsListUI(List<RoomInfo> roomsList)
        {
            const int EMPTY = 0;

            this.Dispatcher.Invoke(() =>
            {
                RoomsList.ItemsSource = null; // reset binding
                RoomsList.ItemsSource = roomsList;
                NoRoomsText.Visibility = (roomsList.Count == EMPTY) ? Visibility.Visible : Visibility.Collapsed;
            });
        }

    }

}

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

        public JoinRoom(Communicator communicator)
        {
            InitializeComponent();
            m_communicator = communicator;
        }

        private void JoinButton_Click(object sender, RoutedEventArgs e)
        {
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

                if (response.rooms.Count == 0)
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
            if (NavigationService.CanGoBack)
            {
                NavigationService.GoBack();
                return;
            }

            MessageBox.Show("Error: There's no previous page you can go back to!");
        }

        private void RoomsList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // TODO v3.0.0
        }
    }

}

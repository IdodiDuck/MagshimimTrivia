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

namespace TriviaClient
{
    /// <summary>
    /// Interaction logic for JoinRoom.xaml
    /// </summary>
    public partial class JoinRoom : Page
    {
        public JoinRoom()
        {
            InitializeComponent();
        }

        private void JoinButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var request = Serializer.SerializeEmptyRequest(RequestCode.RoomsRequest);
                var serverResponse = Globals.Communicator.SendAndReceiveFromServer(request);
                var response = Deserializer.DeserializeResponse<GetRoomsResponse>(serverResponse);

                if (response != null && response.status == StatusCodes.SUCCESS)
                {
                    if (response.rooms.Count > 0)
                    {
                        RoomData selectedRoom = response.rooms[0];

                        var CreateRoomRequest = Serializer.SerializeEmptyRequest(RequestCode.CreateRoomRequest);
                        var ServResponse = Globals.Communicator.SendAndReceiveFromServer(request);
                        var dResponse = Deserializer.DeserializeResponse<GetRoomsResponse>(ServResponse);

                        if (response.status == StatusCodes.SUCCESS)
                        {
                            this.NavigationService.Navigate(
                                new GameLobby(
                                    selectedRoom.name,
                                    (int)selectedRoom.maxPlayers,
                                    (int)selectedRoom.numOfQuestionsInGame,
                                    (int)selectedRoom.timePerQuestion
                                )
                            );
                        }
                        else
                        {
                            MessageBox.Show("Failed to join room, pls try again");
                        }
                    }
                    else
                    {
                        MessageBox.Show("No rooms available. Please create a new room.");
                    }
                }
                else
                {
                    MessageBox.Show("Failed to fetch rooms.");
                }
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

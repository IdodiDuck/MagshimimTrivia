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
using TriviaClient.Infrastructure;
using Newtonsoft.Json;
using TriviaClient.Constants;
using static TriviaClient.Constants.Responses;

namespace TriviaClient
{
    /// <summary>
    /// Interaction logic for MainMenu.xaml
    /// </summary>
    public partial class MainMenu : Page
    {
        private string Username { get; set; } = string.Empty;

        public MainMenu(string username)
        {
            InitializeComponent();
            Username = username;

            UserNameText.Text = Username;
        }

        private void CreateRoomBtn_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new CreateRoom());
        }

        private void JoinRoomBtn_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new JoinRoom());
        }

        private void StatisticsBtn_Click(object sender, RoutedEventArgs e)
        {
            if (NavigationService.CanGoBack)
            {
                var request = Serializer.SerializeEmptyRequest(RequestCode.PersonalStatsRequest);
                Globals.Communicator.SendToServer(request);
                var serverResponse = Globals.Communicator.ReceiveFromServer();
                var response = Deserializer.DeserializeResponse<GetPersonalStatsResponse>(serverResponse);

                if (response == null)
                {
                    MessageBox.Show("Invalid response from server.", "Server Error",
                                  MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }


                if (response.status == StatusCodes.SUCCESS)
                {
                    this.NavigationService.Navigate(new PersonalStatistics(response.statistics));
                }

                return;
            }

            MessageBox.Show("Error: Couldn't fetch personal statistics.");
        }

        private void SignOutBtn_Click(object sender, RoutedEventArgs e)
        {
            if (NavigationService.CanGoBack)
            {
                var request = Serializer.SerializeEmptyRequest(RequestCode.SignoutRequest);
                Globals.Communicator.SendToServer(request);
                var serverResponse = Globals.Communicator.ReceiveFromServer();
                var response = Deserializer.DeserializeResponse<SignOutResponse>(serverResponse);

                if (response == null)
                {
                    MessageBox.Show("Invalid response from server.", "Server Error",
                                  MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }


                if (response.status == StatusCodes.SUCCESS)
                {
                    NavigationService.GoBack();
                }

                return;
            }

            MessageBox.Show("Error: There's no previous page you can go back to!");
        }

        private void BestScoresBtn_click(object sender, RoutedEventArgs e)
        {
            if (NavigationService.CanGoBack)
            {
                var request = Serializer.SerializeEmptyRequest(RequestCode.HighScoreRequest);
                Globals.Communicator.SendToServer(request);
                var serverResponse = Globals.Communicator.ReceiveFromServer();
                var response = Deserializer.DeserializeResponse<GetHighScoreResponse>(serverResponse);

                if (response == null)
                {
                    MessageBox.Show("Invalid response from server.", "Server Error",
                                  MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }


                if (response.status == StatusCodes.SUCCESS)
                {
                    this.NavigationService.Navigate(new HighScores(response.statistics));
                }

                return;
            }

            MessageBox.Show("Error: Couldn't fetch personal statistics.");
        }
    }
}

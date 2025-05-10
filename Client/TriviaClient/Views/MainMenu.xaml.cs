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

        public MainMenu(string username)
        {
            InitializeComponent();
            UserNameText.Text = username;
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
            this.NavigationService.Navigate(new PersonalStatistics());
        }

        private void SignOutBtn_Click(object sender, RoutedEventArgs e)
        {
            this.UserNameText.Text = "User";

            if (NavigationService.CanGoBack)
            {
                const byte REQUEST_CODE = (byte)RequestCode.SignoutRequest;
                string json = JsonConvert.SerializeObject("");
                var request = Serializer.BuildRequest(REQUEST_CODE, json);
                var msg =GlobalCommunicator.Communicator.SendAndReceiveFromServer(request);
                    NavigationService.GoBack();
            }

            else
                MessageBox.Show("Error: There's no previous page you can go back to!");
        }

        private void BestScoresBtn_click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new HighScores());
        }
    }
}

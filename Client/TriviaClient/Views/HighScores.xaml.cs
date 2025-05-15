using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using TriviaClient.Constants;
using TriviaClient.Infrastructure;
using Newtonsoft.Json;
using static TriviaClient.Constants.Responses;

namespace TriviaClient
{
    /// <summary>
    /// Interaction logic for HighScores.xaml
    /// </summary>
    public partial class HighScores : Page
    {
        public List<string>? TopPlayers { get; set; }

        public HighScores()
        {
            InitializeComponent();
            FetchHighScores();
        }

        private void FetchHighScores()
        {
            try
            {
                // Serialize the request to get the high scores
                var request = Serializer.SerializeEmptyRequest(RequestCode.HighScoreRequest);
                Globals.Communicator.SendToServer(request);

                // Receive the response from the server
                var serverResponse = Globals.Communicator.ReceiveFromServer();
                var response = Deserializer.DeserializeResponse<GetHighScoreResponse>(serverResponse);

                // Check if the response is valid
                if (response == null)
                {
                    MessageBox.Show("Invalid response from server.", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }

                // If the response is successful, update the UI with the top players
                if (response.status == StatusCodes.SUCCESS)
                {
                    TopPlayers = response.statistics.Take(3).ToList();  // Get top 3 players
                    DataContext = this;  // Bind the TopPlayers list to the UI
                    return;
                }

                MessageBox.Show("Error fetching high scores.", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }

            catch (Exception ex)
            {
                MessageBox.Show($"Error while fetching high scores: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void BackToMenu_Click(object sender, RoutedEventArgs e)
        {
            if (NavigationService.CanGoBack)
            {
                NavigationService.GoBack();
                return;
            }

            MessageBox.Show("Error: There's no previous page you can go back to!");
        }
    }
}

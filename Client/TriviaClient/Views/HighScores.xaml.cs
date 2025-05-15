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
        private readonly Communicator m_communicator;
        public List<string>? TopPlayers { get; set; }

        public HighScores(Communicator communicator)
        {
            InitializeComponent();
            m_communicator = communicator;
            FetchHighScores();
        }

        private void FetchHighScores()
        {
            try
            {
                var request = Serializer.SerializeEmptyRequest(RequestCode.HighScoreRequest);
                m_communicator.SendToServer(request);

                var serverResponse = m_communicator.ReceiveFromServer();
                var response = Deserializer.DeserializeResponse<GetHighScoreResponse>(serverResponse);

                if (response == null)
                {
                    MessageBox.Show("Invalid response from server.", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }

                if (response.status == StatusCodes.SUCCESS)
                {
                    TopPlayers = response.statistics.Take(3).ToList();
                    DataContext = this;
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

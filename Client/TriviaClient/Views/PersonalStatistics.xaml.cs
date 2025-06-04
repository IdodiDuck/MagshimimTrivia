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
using TriviaClient.Constants;
using static System.Net.Mime.MediaTypeNames;
using static TriviaClient.Constants.Responses;
using TriviaClient.Infrastructure;
using System.IO;
using System.Runtime.Serialization;

namespace TriviaClient
{
    /// <summary>
    /// Interaction logic for PersonalStatistics.xaml
    /// </summary>
    public partial class PersonalStatistics : Page
    {
        private readonly Communicator m_communicator;

        private uint GamesPlayed { get; set; }
        private uint CorrectAnswers { get; set; }
        private uint WrongAnswers { get; set; }
        private double AverageAnswerTime { get; set; }

        public PersonalStatistics(Communicator communicator)
        {
            InitializeComponent();
            m_communicator = communicator;

            this.Loaded += PersonalStatistics_Loaded;
        }

        private void PersonalStatistics_Loaded(object sender, RoutedEventArgs e)
        {
            const int EMPTY = 0;

            try
            {
                var request = Serializer.SerializeEmptyRequest(RequestCode.PersonalStatsRequest);
                m_communicator.SendToServer(request);

                var serverResponse = m_communicator.ReceiveFromServer();
                var response = Deserializer.DeserializeResponse<GetPersonalStatsResponse>(serverResponse);

                if (response == null || response.status != StatusCodes.SUCCESS || response.statistics.Count() == EMPTY)
                {
                    MessageBox.Show("Failed to fetch personal statistics.", "Server Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }

                var correctAnswersLine = response.statistics[3];
                if (correctAnswersLine != null)
                {
                    var parts = correctAnswersLine.Substring("Correct Answers: ".Length).Split('/');
                    if (parts.Length == 2 &&
                        uint.TryParse(parts[0], out uint correct) &&
                        uint.TryParse(parts[1], out uint total))
                    {
                        CorrectAnswers = correct;
                        WrongAnswers = total - correct;
                    }
                }

                GamesPlayed = uint.Parse(response.statistics[2].Substring(response.statistics[2].IndexOf(": ") + 2));
                AverageAnswerTime = double.Parse(response.statistics[5].Substring(response.statistics[5].IndexOf(": ") + 2).Replace("s", ""));

                gamesPlayed.Text = GamesPlayed.ToString();
                correctAns.Text = CorrectAnswers.ToString();
                wrongAns.Text = WrongAnswers.ToString();
                avgAnsTime.Text = AverageAnswerTime.ToString() + "s";
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

        private void BackToMenu_Click(object sender, RoutedEventArgs e)
        {
            if (NavigationService.CanGoBack)
            {
                NavigationService.GoBack();
                return;
            }

            MessageBox.Show("Error: Couldn't fetch personal statistics.");
        }
    }
}

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
using System.Windows.Navigation;
using System.Windows.Shapes;
using TriviaClient.Constants;
using TriviaClient.Infrastructure;
using TriviaClient.Models;

namespace TriviaClient.Views
{
    public partial class AfterGameScore : Page
    {
        private string m_username { get; set; } = string.Empty;
        private readonly Communicator m_communicator;
        private readonly List<PlayerResults> m_results;

        public AfterGameScore(Communicator communicator, List<PlayerResults> results, string username)
        {
            InitializeComponent();

            m_communicator = communicator;
            m_results = results;
            m_username = username;

            LoadResults();
        }

        private void LoadResults()
        {
            try
            {
                const int PLAYERS_AT_PODIUM = 3;
                var players = m_results
                    .Select(r => new PlayerScore
                    {
                        Name = r.username,
                        CorrectAnswers = (int)r.correctAnswerCount,
                        AvgTime = $"{r.averageAnswerTime:F2}"
                    })
                    .OrderByDescending(p => p.CorrectAnswers)
                    .ThenBy(p => double.Parse(p.AvgTime))
                    .ToList();

                var podiumControls = new[] { new { Name = FirstPlaceName, Score = FirstPlaceScore, Time = FirstPlaceTime },
                        new { Name = SecondPlaceName, Score = SecondPlaceScore, Time = SecondPlaceTime },
                        new { Name = ThirdPlaceName, Score = ThirdPlaceScore, Time = ThirdPlaceTime }
                };

                for (int currPlayer = 0; currPlayer < Math.Min(players.Count, PLAYERS_AT_PODIUM); currPlayer++)
                {
                    var player = players[currPlayer];
                    var controls = podiumControls[currPlayer];

                    controls.Name.Text = player.Name;
                    controls.Score.Text = $"Correct: {player.CorrectAnswers}";
                    controls.Time.Text = $"Avg Time: {player.AvgTime}";
                }

                ScoreDataGrid.ItemsSource = players;
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error loading results: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void BackToMenu_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new MainMenu(m_communicator, m_username));
        }
    }
}
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
using static TriviaClient.Constants.Responses;

namespace TriviaClient.Views
{
    public partial class AfterGameScore : Page
    {
        private string m_username { get; set; } = string.Empty;
        private readonly Communicator m_communicator;
        private readonly List<PlayerResults> m_results;

        public class PlayerScore
        {
            public string Name { get; set; } = string.Empty;
            public int CorrectAnswers { get; set; }
            public string AvgTime { get; set; } = string.Empty;
        }

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

                if (players.Count > 0)
                {
                    FirstPlaceName.Text = players[0].Name;
                    FirstPlaceScore.Text = $"Correct: {players[0].CorrectAnswers}";
                    FirstPlaceTime.Text = $"Avg Time: {players[0].AvgTime}";
                }

                if (players.Count > 1)
                {
                    SecondPlaceName.Text = players[1].Name;
                    SecondPlaceScore.Text = $"Correct: {players[1].CorrectAnswers}";
                    SecondPlaceTime.Text = $"Avg Time: {players[1].AvgTime}";
                }

                if (players.Count > 2)
                {
                    ThirdPlaceName.Text = players[2].Name;
                    ThirdPlaceScore.Text = $"Correct: {players[2].CorrectAnswers}";
                    ThirdPlaceTime.Text = $"Avg Time: {players[2].AvgTime}";
                }

                ScoreDataGrid.ItemsSource = players;
            }
            catch (System.Exception ex)
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
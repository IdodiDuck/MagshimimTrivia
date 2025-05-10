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

namespace TriviaClient
{
    public class PlayerScore
    {
        public string Name { get; set; } = string.Empty;
        public int Score { get; set; }
    }

    /// <summary>
    /// Interaction logic for HighScores.xaml
    /// </summary>
    public partial class HighScores : Page
    {
        public List<PlayerScore> TopPlayers { get; set; }

        public HighScores(List<string> statistics)
        {
            InitializeComponent();

            TopPlayers = new List<PlayerScore>();

            foreach (var stat in statistics)
            {
                var parts = stat.Split(new[] { " - Score: " }, System.StringSplitOptions.None);
                if (parts.Length == 2 && int.TryParse(parts[1], out int score))
                {
                    TopPlayers.Add(new PlayerScore { Name = parts[0], Score = score });
                }
            }

            DataContext = this;
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

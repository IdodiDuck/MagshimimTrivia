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

    /// <summary>
    /// Interaction logic for HighScores.xaml
    /// </summary>
    public partial class HighScores : Page
    {
        public List<string>? TopPlayers { get; set; }

        public HighScores(List<string> statistics)
        {
            const int TOP_THREE = 3;

            InitializeComponent();

            TopPlayers = statistics.Take(TOP_THREE).ToList();
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

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

namespace TriviaClient
{
    /// <summary>
    /// Interaction logic for MainMenu.xaml
    /// </summary>
    public partial class MainMenu : Page
    {

        public MainMenu()
        {
            InitializeComponent();
        }

        private void CreateRoomBtn_Click(object sender, RoutedEventArgs e)
        {
            // go to CreateRoom
        }

        private void JoinRoomBtn_Click(object sender, RoutedEventArgs e)
        {
            // go to JoinRoom
        }

        private void StatisticsBtn_Click(object sender, RoutedEventArgs e)
        {
            // go to PersonalStatistics
        }

        private void SignOutBtn_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new Uri("/TriviaClient;component/Views/Login.xaml", UriKind.Relative));
        }

        private void BestScoresBtn_click(object sender, RoutedEventArgs e)
        {
            // go to HighScores
        }
    }
}

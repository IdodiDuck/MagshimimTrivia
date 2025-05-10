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
    /// Interaction logic for GameLobby.xaml
    /// </summary>
    /// 


    public partial class GameLobby : Page
    {
        private string RoomName { get; set; } = string.Empty;
        private int MaxPlayer { get; set; }
        private int QuestionTime { get; set; }
        private int TimePerQuestion { get; set; }

        public GameLobby(string roomName, int maxPlayer, int questionTime, int timePerQuestion)
        {
            InitializeComponent();
            RoomName = roomName;
            MaxPlayer = maxPlayer;
            QuestionTime = questionTime;
            TimePerQuestion = timePerQuestion;
        }

        private void StartGameButton_Click(object sender, RoutedEventArgs e)
        {

        }

        private void LeaveButton_Click(object sender, RoutedEventArgs e)
        {
            // Send to Server LeaveRoom Request!

            if (NavigationService.CanGoBack)
            {
                NavigationService.GoBack();
                return;
            }

            MessageBox.Show("Error: There's no previous page you can go back to!");
        }
    }
}

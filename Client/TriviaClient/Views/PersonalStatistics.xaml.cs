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

namespace TriviaClient
{
    /// <summary>
    /// Interaction logic for PersonalStatistics.xaml
    /// </summary>
    public partial class PersonalStatistics : Page
    {
        private uint GamesPlayed { get; set; }
        private uint CorrectAnswers { get; set; }
        private uint WrongAnswers { get; set; }
        private int AverageAnswerTime { get; set; }


        public PersonalStatistics(List<string> statistics)
        {
            var correctAnswersLine = statistics[3];
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

            InitializeComponent();
            GamesPlayed = uint.Parse(statistics[2].Substring(statistics[2].IndexOf(": ") + 2));
            AverageAnswerTime = int.Parse(statistics[5].Substring(statistics[5].IndexOf(": ") + 2).Replace("s", ""));

            gamesPlayed.Text = GamesPlayed.ToString();
            correctAns.Text = CorrectAnswers.ToString();
            wrongAns.Text = WrongAnswers.ToString();
            avgAnsTime.Text = AverageAnswerTime.ToString() + "s";
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

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

using TriviaClient.Infrastructure;

namespace TriviaClient.Views
{
    /// <summary>
    /// Interaction logic for Game.xaml
    /// </summary>
    public partial class Game : Page
    {
        private readonly Communicator m_communicator;
        private uint QuestionAmount { get; set; }
        private uint TimePerQuestion { get; set; }
        public Game(Communicator communicator, uint questionsAmount, uint timePerQuestions)
        {
            InitializeComponent();

            m_communicator = communicator;
            QuestionAmount = questionsAmount;
            TimePerQuestion = timePerQuestions;
        }
    }
}

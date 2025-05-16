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
using TriviaClient.Infrastructure;

namespace TriviaClient
{

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly Communicator m_communicator;
        public MainWindow()
        {
            InitializeComponent();
            m_communicator = new Communicator();
            NavigateToLogin();
        }

        private void NavigateToLogin()
        {
            MainFrame.Navigate(new Login(m_communicator));
        }

        private void MainWindow_Closing(object? sender, System.ComponentModel.CancelEventArgs e)
        {
            try
            {
                m_communicator.CloseConnection();
            }

            catch (Exception ex)
            {
                MessageBox.Show($"Error closing connection: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

    }
}

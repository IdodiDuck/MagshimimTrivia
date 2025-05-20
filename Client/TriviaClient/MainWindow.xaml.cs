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
using static TriviaClient.Constants.Responses;

namespace TriviaClient
{
    public static class Globals
    {
        public static Communicator Communicator = new Communicator();
    }

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        public MainWindow()
        {
            InitializeComponent();
            InitializeCommunicator();
            NavigateToLogin();
        }

        private void InitializeCommunicator()
        {
            try
            {
                Globals.Communicator = new Communicator();

                if (!Globals.Communicator.IsConnected)
                {
                    MessageBox.Show("Failed to connect to server. Please try again later.",
                                  "Connection Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    System.Environment.Exit(1);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Failed to initialize connection: {ex.Message}",
                              "Connection Error", MessageBoxButton.OK, MessageBoxImage.Error);
                System.Environment.Exit(1);
            }
        }

        private void NavigateToLogin()
        {
            try
            {
                var loginPage = new Login();
                MainFrame.Navigate(loginPage);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Failed to load login page: {ex.Message}",
                              "Navigation Error", MessageBoxButton.OK, MessageBoxImage.Error);
                System.Environment.Exit(1);
            }
        }
    }
}

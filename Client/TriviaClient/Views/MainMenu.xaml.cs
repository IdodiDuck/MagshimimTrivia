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
using Newtonsoft.Json;
using TriviaClient.Constants;
using static TriviaClient.Constants.Responses;
using System.IO;
using System.Runtime.Serialization;

namespace TriviaClient
{
    /// <summary>
    /// Interaction logic for MainMenu.xaml
    /// </summary>
    public partial class MainMenu : Page
    {
        private string Username { get; set; } = string.Empty;
        private readonly Communicator m_communicator;

        public MainMenu(Communicator clientCommunicator, string username)
        {
            InitializeComponent();
            Username = username;
            m_communicator = clientCommunicator;

            UserNameText.Text = Username;
        }

        private void CreateRoomBtn_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new CreateRoom(m_communicator));
        }

        private void JoinRoomBtn_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new JoinRoom(m_communicator));
        }

        private void StatisticsBtn_Click(object sender, RoutedEventArgs e)
        {
           this.NavigationService.Navigate(new PersonalStatistics(m_communicator));
        }

        private void SignOutBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var request = Serializer.SerializeEmptyRequest(RequestCode.SignoutRequest);
                m_communicator.SendToServer(request);
                byte[] serverResponse = m_communicator.ReceiveFromServer();
                var response = Deserializer.DeserializeResponse<SignOutResponse>(serverResponse);

                if (response == null)
                {
                    MessageBox.Show("Invalid response from server or sign out failed.", "Server Error",
                                      MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }    
                
                if (serverResponse[NetworkConstants.CODE_INDEX] == (byte)(ResponseCode.ERROR_RESPONSE))
                {
                    ErrorResponse? errorResponse = Deserializer.DeserializeResponse<ErrorResponse>(serverResponse);
                    MessageBox.Show(errorResponse?.message, "Error", MessageBoxButton.OK, MessageBoxImage.Information);
                    return;
                }

                if (NavigationService.CanGoBack)
                {
                    NavigationService.GoBack();
                    return;
                }
            }

            catch (IOException ex)
            {
                MessageBox.Show($"Connection Error: {ex.Message}", "Network Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }

            catch (SerializationException ex)
            {
                MessageBox.Show($"Data serialization error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }

            catch (Exception ex)
            {
                MessageBox.Show($"An unexpected error occurred: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }

            MessageBox.Show("Error: There's No previous page to navigate back to.");
        }

        private void BestScoresBtn_click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new HighScores(m_communicator));
        }
    }
}

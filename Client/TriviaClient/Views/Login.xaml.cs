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
using static TriviaClient.Constants.Responses;
using TriviaClient.Constants;

using TriviaClient.Infrastructure;
using System.Net.Sockets;
using System.Runtime.Serialization;
using System.Diagnostics;

namespace TriviaClient
{
    /// <summary>
    /// Interaction logic for Login.xaml
    /// </summary>
    public partial class Login : Page
    {
        private readonly Communicator m_communicator;

        public Login(Communicator communicator)
        {
            InitializeComponent();
            m_communicator = communicator;
        }

        private void LoginButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var loginRequest = new LoginRequest
                {
                    username = UsernameTextBox.Text,
                    password = PasswordBox.Password
                };

                byte[] serializedRequest = Serializer.SerializeRequest(loginRequest);
                m_communicator.SendToServer(serializedRequest);
                byte[] serverResponse = m_communicator.ReceiveFromServer();
                var response = Deserializer.DeserializeResponse<LoginResponse>(serverResponse);

                if (response == null)
                {
                    MessageBox.Show("Invalid response from server.", "Server Error",
                                  MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }

                if (response.status == StatusCodes.SUCCESS)
                {
                    UsernameTextBox.Clear();
                    PasswordBox.Clear();

                    this.NavigationService.Navigate(new MainMenu(m_communicator, loginRequest.username)); // העבר הלאה
                }
                else
                {
                    MessageBox.Show("Invalid username or password / user already logged in.", "Login Failed",
                                   MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
            catch (SocketException ex)
            {
                MessageBox.Show($"Connection error: {ex.Message}", "Network Error",
                              MessageBoxButton.OK, MessageBoxImage.Error);
            }
            catch (SerializationException ex)
            {
                MessageBox.Show($"Data serialization error: {ex.Message}", "Error",
                              MessageBoxButton.OK, MessageBoxImage.Error);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"An unexpected error occurred: {ex.Message}", "Error",
                              MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void Input_TextChanged(object sender, TextChangedEventArgs e)
        {
            UpdateLoginButtonState();
        }

        private void PasswordBox_PasswordChanged(object sender, RoutedEventArgs e)
        {
            UpdateLoginButtonState();
        }

        private void UpdateLoginButtonState()
        {
            LoginButton.IsEnabled = !string.IsNullOrWhiteSpace(UsernameTextBox.Text) &&
                                   !string.IsNullOrWhiteSpace(PasswordBox.Password);
        }

        private void SignUp_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            this.NavigationService.Navigate(new Signup(m_communicator));
        }
    }
}

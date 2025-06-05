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
using System.IO;

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
                    ShowError("Invalid response from server.");
                    return;
                }

                if (serverResponse[NetworkConstants.CODE_INDEX] == (byte)(ResponseCode.ERROR_RESPONSE))
                {
                    ErrorResponse? errorResponse = Deserializer.DeserializeResponse<ErrorResponse>(serverResponse);
                    ShowError(errorResponse?.message ?? "Error.");
                    return;
                }

                if (response.status == StatusCodes.SUCCESS)
                {
                    UsernameTextBox.Clear();
                    PasswordBox.Clear();

                    this.NavigationService.Navigate(new MainMenu(m_communicator, loginRequest.username));
                }

                else
                {
                    ShowError($"Error: Login Failed");
                }
            }

            catch (IOException ex)
            {
                ShowError($"Connection Error: {ex.Message}");
            }

            catch (SerializationException ex)
            {
                ShowError($"Data serialization error: {ex.Message}");
            }

            catch (Exception ex)
            {
                ShowError($"An unexpected error occurred: {ex.Message}");
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

        private void ShowError(string message)
        {
            ErrorTextBlock.Text = message;
            ErrorTextBlock.Visibility = Visibility.Visible;
        }
    }
}

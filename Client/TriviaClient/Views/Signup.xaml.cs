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
using static TriviaClient.Constants.Responses;
using System.Net.Sockets;
using System.Runtime.Serialization;
using TriviaClient.Constants;
using TriviaClient.Infrastructure;
using System.IO;

namespace TriviaClient
{
    /// <summary>
    /// Interaction logic for Signup.xaml
    /// </summary>
    public partial class Signup : Page
    {
        private readonly Communicator m_communicator;

        public Signup(Communicator communicator)
        {
            InitializeComponent();
            m_communicator = communicator;
        }

        private void SignUpButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var signupRequest = new SignupRequest
                {
                    username = UsernameTextBox.Text,
                    password = PasswordBox.Password,
                    email = EmailTextBox.Text
                };

                byte[] serializedRequest = Serializer.SerializeRequest(signupRequest);
                m_communicator.SendToServer(serializedRequest);
                byte[] serverResponse = m_communicator.ReceiveFromServer();
                var response = Deserializer.DeserializeResponse<SignupResponse>(serverResponse);

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
                    this.NavigationService.Navigate(new MainMenu(m_communicator, UsernameTextBox.Text));
                }

                else
                {
                    ShowError("Error: Signup Failed");
                }
            }

            catch (IOException ex)
            {
                ShowError($"Connection error: {ex.Message}");
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

        private void InputUsername_TextChanged(object sender, TextChangedEventArgs e)
        {
            UpdateSignupButtonState();
        }

        private void InputEmail_TextChanged(object sender, TextChangedEventArgs e)
        {
            UpdateSignupButtonState();
        }

        private void PasswordBox_PasswordChanged(object sender, RoutedEventArgs e)
        {
            UpdateSignupButtonState();
        }

        private void UpdateSignupButtonState()
        {
            SignUpButton.IsEnabled = !string.IsNullOrWhiteSpace(UsernameTextBox.Text) &&
                                   !string.IsNullOrWhiteSpace(PasswordBox.Password) &&
                                   !string.IsNullOrWhiteSpace(EmailTextBox.Text);
        }

        private void Login_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            this.NavigationService.Navigate(new Login(m_communicator));
        }

        private void ShowError(string message)
        {
            ErrorTextBlock.Text = message;
            ErrorTextBlock.Visibility = Visibility.Visible;
        }
    }
}

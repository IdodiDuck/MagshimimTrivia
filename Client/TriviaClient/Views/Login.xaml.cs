using System;
using System.IO;
using System.Runtime.Serialization;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using TriviaClient.Constants;
using TriviaClient.Infrastructure;
using static TriviaClient.Constants.Responses;

namespace TriviaClient
{
    public partial class Login : Page
    {
        private readonly Communicator m_communicator;
        private bool isChangingProgrammatically = false;

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
                    password = ShowPasswordCheckBox.IsChecked == true ? PasswordTextBox.Text : PasswordBox.Password
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
                    PasswordTextBox.Clear();
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
            if (isChangingProgrammatically) return;
            isChangingProgrammatically = true;
            PasswordTextBox.Text = PasswordBox.Password;
            isChangingProgrammatically = false;
            UpdateLoginButtonState();
        }

        private void PasswordTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (isChangingProgrammatically) return;
            isChangingProgrammatically = true;
            PasswordBox.Password = PasswordTextBox.Text;
            isChangingProgrammatically = false;
            UpdateLoginButtonState();
        }

        private void ShowPasswordCheckBox_Checked(object sender, RoutedEventArgs e)
        {
            PasswordTextBox.Visibility = Visibility.Visible;
            PasswordBox.Visibility = Visibility.Collapsed;
        }

        private void ShowPasswordCheckBox_Unchecked(object sender, RoutedEventArgs e)
        {
            PasswordBox.Visibility = Visibility.Visible;
            PasswordTextBox.Visibility = Visibility.Collapsed;
        }

        private void UpdateLoginButtonState()
        {
            string password = ShowPasswordCheckBox.IsChecked == true ? PasswordTextBox.Text : PasswordBox.Password;
            LoginButton.IsEnabled = !string.IsNullOrWhiteSpace(UsernameTextBox.Text) && !string.IsNullOrWhiteSpace(password);
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

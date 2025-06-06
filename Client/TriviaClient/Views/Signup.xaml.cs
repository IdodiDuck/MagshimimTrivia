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
    public partial class Signup : Page
    {
        private readonly Communicator m_communicator;
        private bool isChangingProgrammatically = false;

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
                    password = ShowPasswordCheckBox.IsChecked == true ? PasswordTextBox.Text : PasswordBox.Password,
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
            if (isChangingProgrammatically) return;
            isChangingProgrammatically = true;
            PasswordTextBox.Text = PasswordBox.Password;
            isChangingProgrammatically = false;
            UpdateSignupButtonState();
        }

        private void PasswordTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (isChangingProgrammatically) return;
            isChangingProgrammatically = true;
            PasswordBox.Password = PasswordTextBox.Text;
            isChangingProgrammatically = false;
            UpdateSignupButtonState();
        }

        private void ShowPasswordCheckBox_Checked(object sender, RoutedEventArgs e)
        {
            PasswordTextBox.Text = PasswordBox.Password;
            PasswordBox.Visibility = Visibility.Collapsed;
            PasswordTextBox.Visibility = Visibility.Visible;
        }

        private void ShowPasswordCheckBox_Unchecked(object sender, RoutedEventArgs e)
        {
            PasswordBox.Password = PasswordTextBox.Text;
            PasswordTextBox.Visibility = Visibility.Collapsed;
            PasswordBox.Visibility = Visibility.Visible;
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

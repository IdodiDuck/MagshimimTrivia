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

namespace TriviaClient
{
    /// <summary>
    /// Interaction logic for Signup.xaml
    /// </summary>
    public partial class Signup : Page
    {
        public Signup()
        {
            InitializeComponent();
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
                byte[] serverResponse = GlobalCommunicator.Communicator.SendAndReceiveFromServer(serializedRequest);
                var response = Deserializer.DeserializeResponse<LoginResponse>(serverResponse);

                if (response == null)
                {
                    MessageBox.Show("Invalid response from server.", "Server Error",
                                  MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }

                if (response.status == StatusCodes.SUCCESS)
                {
                    this.NavigationService.Navigate(new MainMenu(UsernameTextBox.Text));
                }

                else
                {
                    MessageBox.Show("Invalid username or password.", "Signup Failed",
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
            if (NavigationService.CanGoBack)
            {
                NavigationService.GoBack();
                return;
            }

            MessageBox.Show("Error: There's no previous page you can go back to!");
        }
    }
}

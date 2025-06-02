using System;
using System.Collections.Generic;
using System.IO;
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
using System.Windows.Threading;
using TriviaClient.Constants;
using TriviaClient.Infrastructure;
using static TriviaClient.Constants.Responses;

namespace TriviaClient.Views
{
    /// <summary>
    /// Interaction logic for Game.xaml
    /// </summary>
    public partial class Game : Page
    {
        private readonly Communicator m_communicator;
        private uint QuestionAmount { get; set; }
        private uint CurrentQuestionNumber {  get; set; }
        private uint CorrectAnswersAmount { get; set; }
        private uint? SelectedAnswerId { get; set; } = null;
        private List<KeyValuePair<string, string>> answers = new();
        private uint TimePerQuestion { get; set; }
        private DispatcherTimer QuestionTimer;
        private int TimeLeft;
        private string m_username { get; set; } = string.Empty;

        public Game(Communicator communicator, uint questionsAmount, uint timePerQuestions, string username)
        {
            InitializeComponent();

            m_username = username;

            this.m_communicator = communicator;
            this.QuestionAmount = questionsAmount;
            this.TimePerQuestion = timePerQuestions;
            this.CorrectAnswersAmount = 0;

            this.CurrentQuestionNumber = 0;
            this.TimeLeft = (int)(timePerQuestions);
            this.QuestionTimer = new DispatcherTimer();
            this.QuestionTimer.Interval = TimeSpan.FromSeconds(1);
            this.QuestionTimer.Tick += QuestionTimer_Tick;

            StartQuestionTimer((int)(timePerQuestions));
            GetNextQuestionFromServer();

        }

        private void StartQuestionTimer(int seconds)
        {
            this.TimeLeft = seconds;
            TimeLeftText.Text = this.TimeLeft.ToString();
            this.QuestionTimer.Start();
        }
        private void QuestionTimer_Tick(object? sender, EventArgs e)
        {
            this.TimeLeft--;
            TimeLeftText.Text = $"{this.TimeLeft} seconds";

            if (this.TimeLeft <= 0)
            {
                uint answerToSend = SelectedAnswerId ?? uint.MaxValue;
                _ = SubmitAnswerAsync(answerToSend);
            }
        }

        private void GetNextQuestionFromServer()
        {
            try
            {
                m_communicator.SendToServer(Serializer.SerializeEmptyRequest(RequestCode.GET_QUESTION_REQUEST));
                var questionResponse = Deserializer.DeserializeResponse<GetQuestionResponse>(m_communicator.ReceiveFromServer());

                if (questionResponse == null)
                {
                    MessageBox.Show("Failed to get question from server", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }

                if (questionResponse.status == StatusCodes.SUCCESS)
                {
                    this.CurrentQuestionNumber++;
                    this.QuestionsProgressText.Text = $"{this.CurrentQuestionNumber}/{this.QuestionAmount}";
                    DisplayQuestion(questionResponse);
                    return;
                }

                MessageBox.Show("Failed to get question from server", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }

            catch (Exception ex)
            {
                MessageBox.Show($"Error while getting question: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void DisplayQuestion(GetQuestionResponse questionResponse)
        {
            CurrentQuestion.Text = questionResponse.question;

            this.answers = questionResponse.answers.ToList();

            AnswerButton1.Content = this.answers[0].Value;
            AnswerButton2.Content = this.answers[1].Value;
            AnswerButton3.Content = this.answers[2].Value;
            AnswerButton4.Content = this.answers[3].Value;

            AnswerButton1.IsEnabled = true;
            AnswerButton2.IsEnabled = true;
            AnswerButton3.IsEnabled = true;
            AnswerButton4.IsEnabled = true;

            this.SelectedAnswerId = null;
            ClearButtonHighlights();
            ResetButtonColors();

            StartQuestionTimer((int)(this.TimePerQuestion));
        }

        private void ExitButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                this.m_communicator.SendToServer(Serializer.SerializeEmptyRequest(RequestCode.LEAVE_GAME_REQUEST));
                var serverResponse = Deserializer.DeserializeResponse<LeaveGameResponse>(this.m_communicator.ReceiveFromServer());

                if (serverResponse?.status == StatusCodes.SUCCESS)
                {
                    this.QuestionTimer.Stop();
                    while (NavigationService.CanGoBack)
                    {
                        NavigationService.GoBack();
                    }

                    NavigationService.GoForward();
                    return;
                }

                MessageBox.Show("There's no previous page to return to.", "Navigation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
            }

            catch (IOException ex)
            {
                MessageBox.Show($"Connection Error: {ex.Message}", "Network Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }

            catch (Exception ex)
            {
                MessageBox.Show($"Unexpected error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }

        }

        private void DisableAnswerButtons()
        {
            AnswerButton1.IsEnabled = false;
            AnswerButton2.IsEnabled = false;
            AnswerButton3.IsEnabled = false;
            AnswerButton4.IsEnabled = false;
        }

        private void HighlightSelectedButton(Button selected)
        {
            var buttons = new[] { AnswerButton1, AnswerButton2, AnswerButton3, AnswerButton4 };

            foreach (var btn in buttons)
            {
                btn.BorderThickness = new Thickness(1);
                btn.BorderBrush = Brushes.Black;
            }

            selected.BorderThickness = new Thickness(4);
            selected.BorderBrush = Brushes.DodgerBlue;
        }

        private void ClearButtonHighlights()
        {
            var buttons = new[] { AnswerButton1, AnswerButton2, AnswerButton3, AnswerButton4 };

            foreach (var btn in buttons)
            {
                btn.BorderThickness = new Thickness(1);
                btn.BorderBrush = Brushes.Black;
            }

        }
        private async Task WaitForGameToEndAsync()
        {
            const int TWO_SECONDS = 2000;
            try
            {
                while (true)
                {
                    await Task.Delay(TWO_SECONDS);

                    m_communicator.SendToServer(Serializer.SerializeEmptyRequest(RequestCode.GET_GAME_RESULTS_REQUEST));
                    var responseBytes = m_communicator.ReceiveFromServer();
                    var resultsResponse = Deserializer.DeserializeResponse<GetGameResultsResponse>(responseBytes);

                    if (resultsResponse?.status == StatusCodes.SUCCESS)
                    {
                        Application.Current.Dispatcher.Invoke(() =>
                        {
                            var navService = NavigationService.GetNavigationService(this);

                            if (navService != null)
                            {
                                this.NavigationService.Navigate(new AfterGameScore(m_communicator, resultsResponse.results, m_username));
                                return;
                            }

                            else
                            {
                                MessageBox.Show("NavigationService not available", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                            }
                        });

                        break;
                    }
                }
            }

            catch (Exception ex)
            {
                Application.Current.Dispatcher.Invoke(() =>
                {
                    MessageBox.Show($"Error getting game results: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                });
            }
        }

        private async Task SubmitAnswerAsync(uint answerId)
        {
            try
            {
                this.QuestionTimer.Stop();

                var answerRequest = new SubmitAnswerRequest { answerId = answerId };
                var requestBytes = Serializer.SerializeRequest(answerRequest);
                m_communicator.SendToServer(requestBytes);

                var responseBytes = m_communicator.ReceiveFromServer();
                var response = Deserializer.DeserializeResponse<SubmitAnswerResponse>(responseBytes);

                if (response == null)
                {
                    MessageBox.Show("Failed to submit answer", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }

                if (response.status != StatusCodes.SUCCESS)
                {
                    MessageBox.Show("Failed to submit answer", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }

                bool isAnswerCorrect = response.correctAnswerId == answerId;

                this.CorrectAnswersText.Text = isAnswerCorrect ? $"{++this.CorrectAnswersAmount}" : $"{this.CorrectAnswersAmount}";

                ShowAnswerResult(answerId, response.correctAnswerId);
                await Task.Delay(1500);

                DisableAnswerButtons();

                if (this.CurrentQuestionNumber != this.QuestionAmount)
                {
                    GetNextQuestionFromServer();
                    return;
                }

                ShowWaitingForGameToEndUI();
                _ = WaitForGameToEndAsync();
                return;
            }

            catch (Exception ex)
            {
                MessageBox.Show($"Error submitting answer: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void ShowAnswerResult(uint selectedId, uint correctAnswerId)
        {
            const string SELECTED_BUTTON = "selected", CORRECT_BUTTONS = "correct";

            var buttons = new[] { AnswerButton1, AnswerButton2, AnswerButton3, AnswerButton4 };

            for (int currButton = 0; currButton < buttons.Length; currButton++)
            {
                var button = buttons[currButton];
                var answerKey = uint.Parse(this.answers[currButton].Key);

                string buttonState = answerKey == correctAnswerId ? CORRECT_BUTTONS :
                               answerKey == selectedId ? SELECTED_BUTTON : "default";

                switch (buttonState)
                {
                    case CORRECT_BUTTONS:
                        button.BorderBrush = Brushes.LightGreen;
                        button.BorderThickness = new Thickness(3);
                        break;

                    case SELECTED_BUTTON:
                        button.BorderBrush = Brushes.IndianRed;
                        button.BorderThickness = new Thickness(3);
                        break;

                    default:
                        button.BorderBrush = Brushes.Black;
                        button.BorderThickness = new Thickness(1);
                        break;
                }
            }
        }

        private void ResetButtonColors()
        {
            var buttons = new[] { AnswerButton1, AnswerButton2, AnswerButton3, AnswerButton4 };

            foreach (var button in buttons)
            {
                button.Background = Brushes.White;
            }
        }

        private void ShowWaitingForGameToEndUI()
        {
            GameGrid.Visibility = Visibility.Collapsed;
            WaitingOverlay.Visibility = Visibility.Visible;
        }
        private void AnswerButton1_Click(object sender, RoutedEventArgs e)
        {
            this.SelectedAnswerId = uint.Parse(this.answers[0].Key);
            HighlightSelectedButton(AnswerButton1);
        }

        private void AnswerButton2_Click(object sender, RoutedEventArgs e)
        {
            this.SelectedAnswerId = uint.Parse(this.answers[1].Key);
            HighlightSelectedButton(AnswerButton2);
        }

        private void AnswerButton3_Click(object sender, RoutedEventArgs e)
        {
            this.SelectedAnswerId = uint.Parse(this.answers[2].Key);
            HighlightSelectedButton(AnswerButton3);
        }

        private void AnswerButton4_Click(object sender, RoutedEventArgs e)
        {
            this.SelectedAnswerId = uint.Parse(this.answers[3].Key);
            HighlightSelectedButton(AnswerButton4);
        }
    }
}

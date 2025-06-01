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
        private Button? SelectedButton { get; set; } = null;
        private uint TimePerQuestion { get; set; }
        private DispatcherTimer QuestionTimer;
        private int TimeLeft;

        public Game(Communicator communicator, uint questionsAmount, uint timePerQuestions)
        {
            InitializeComponent();

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
                SubmitAnswer(answerToSend);
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

            var answers = questionResponse.answers.ToList();

            AnswerButton1.Content = answers[0].Value;
            AnswerButton2.Content = answers[1].Value;
            AnswerButton3.Content = answers[2].Value;
            AnswerButton4.Content = answers[3].Value;

            AnswerButton1.IsEnabled = true;
            AnswerButton2.IsEnabled = true;
            AnswerButton3.IsEnabled = true;
            AnswerButton4.IsEnabled = true;

            this.SelectedAnswerId = null;
            ClearButtonHighlights();

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
            this.SelectedButton = selected;
        }

        private void ClearButtonHighlights()
        {
            var buttons = new[] { AnswerButton1, AnswerButton2, AnswerButton3, AnswerButton4 };

            foreach (var btn in buttons)
            {
                btn.BorderThickness = new Thickness(1);
                btn.BorderBrush = Brushes.Black;
            }

            SelectedButton = null;
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
                                //navService.Navigate(new GameResultsPage(resultsResponse));
                                while (NavigationService.CanGoBack)
                                {
                                    NavigationService.GoBack();
                                }

                                NavigationService.GoForward();
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

        private void SubmitAnswer(uint answerId)
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
                if (this.CurrentQuestionNumber != this.QuestionAmount)
                {
                    GetNextQuestionFromServer();
                    return;
                }

                _ = WaitForGameToEndAsync();

                // TODO -> Move this eventually to the Game Results Page
                return;
            }

            catch (Exception ex)
            {
                MessageBox.Show($"Error submitting answer: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }
        private void AnswerButton1_Click(object sender, RoutedEventArgs e)
        {
            SelectedAnswerId = 0;
            HighlightSelectedButton(AnswerButton1);
            DisableAnswerButtons();
        }

        private void AnswerButton2_Click(object sender, RoutedEventArgs e)
        {
            SelectedAnswerId = 1;
            HighlightSelectedButton(AnswerButton2);
            DisableAnswerButtons();
        }

        private void AnswerButton3_Click(object sender, RoutedEventArgs e)
        {
            SelectedAnswerId = 2;
            HighlightSelectedButton(AnswerButton3);
            DisableAnswerButtons();
        }

        private void AnswerButton4_Click(object sender, RoutedEventArgs e)
        {
            SelectedAnswerId = 3;
            HighlightSelectedButton(AnswerButton4);
            DisableAnswerButtons();
        }
    }
}

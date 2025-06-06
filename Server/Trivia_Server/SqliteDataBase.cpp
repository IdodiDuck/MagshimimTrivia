#include "SqliteDataBase.h"

#include <io.h>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <random>

#include <curl/curl.h> // Used for processing HTTP Responses

typedef struct scoresEntity
{
    SqliteDataBase* db;
    std::vector<std::pair<std::string, int>>* scores;

} scoresEntity;

SqliteDataBase::SqliteDataBase() : _dataBaseName("trivia.db"), _dataBase(nullptr)
{

}

SqliteDataBase::~SqliteDataBase()
{
    close(); // If close() wasn't called, the Database will still close connection!
}

bool SqliteDataBase::open()
{
    const int DATA_BASE_EXISTS_CODE = 0;

    if (this->_dataBase)
    {
        std::cerr << "Error: Trivia DataBase is already open!\n";
        return false;
    }

    int dbExists = _access(this->_dataBaseName.c_str(), DATA_BASE_EXISTS_CODE);
    int result = sqlite3_open(this->_dataBaseName.c_str(), &this->_dataBase);

    if (result != SQLITE_OK)
    {
        std::cerr << "Error: Cannot open database: " << std::string(sqlite3_errmsg(this->_dataBase)) << std::endl;
        return false;
    }

    const std::string ACTIVATE_FOREIGN_KEYS = "PRAGMA foreign_keys = ON;";

    // Activate foreign key constraints (Should be relevant when we add additional tables which use the USERNAME as key)
    if (executeQuery(ACTIVATE_FOREIGN_KEYS.c_str()) == static_cast<int>(DatabaseResult::DATABASE_ERROR))
    {
        return false;
    }

    if (dbExists != DATA_BASE_EXISTS_CODE)
    {
        try
        {
            initializeTriviaDB();
        }

        catch (const std::runtime_error& e)
        {
            std::cerr << "DataBase: [ERROR] during database initialization: " << e.what() << std::endl;
            close();
            return false;
        }
    }

    std::cout << "DataBase: Database opened successfully!" << std::endl;
    return true;
}

bool SqliteDataBase::close()
{
    std::cout << "DataBase: Attempting to close database..." << std::endl;
    if (!isDataBaseOpen())
    {
        std::cerr << "DataBase: [ERROR]: Trivia DataBase is not currently open!\n";
        return false;
    }

    int result = sqlite3_close(this->_dataBase);

    if (result != SQLITE_OK)
    {
        std::cerr << "DataBase: [ERROR]: Failed to close database: " << sqlite3_errmsg(this->_dataBase) << "\n";
        this->_dataBase = nullptr;
        return false;
    }

    this->_dataBase = nullptr;
    std::cout << "DataBase: Database closed successfully!" << std::endl;
    return true;
}

int SqliteDataBase::doesUserExist(const std::string& user)
{
    if (!isDataBaseOpen())
    {
        std::cerr << "DataBase: [ERROR] Database isn't open!" << std::endl;
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    std::string SQLQuery = "SELECT USERNAME FROM USERS WHERE USERNAME = '" + user + "';";

    char* errMsg = nullptr;
    int userExists = 0;

    auto callback = [](void* data, int argc, char** argv, char** colNames) -> int
        {
            const int EMPTY = 0, SUCCESSFULL_EXECUTION = 0;
            int* userExists = static_cast<int*>(data);

            if ((argc > EMPTY) && (argv[0]))
            {
                *userExists = static_cast<int>(DatabaseResult::USER_EXISTS); // found the user
            }

            return SUCCESSFULL_EXECUTION;
        };

    if (sqlite3_exec(this->_dataBase, SQLQuery.c_str(), callback, &userExists, &errMsg) != SQLITE_OK)
    {
        std::cerr << "DataBase: [ERROR]: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    return (userExists == static_cast<int>(DatabaseResult::USER_EXISTS)) ? userExists : static_cast<int>(DatabaseResult::USER_NOT_FOUND);
}

int SqliteDataBase::doesPasswordMatch(const std::string& user, const std::string& password)
{
    if (!(isDataBaseOpen()))
    {
        std::cerr << "DataBase: [ERROR]: Database isn't open!" << std::endl;
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    if ((user.empty()) || (password.empty()))
    {
        std::cerr << "DataBase: [ERROR]: Empty username or password provided!\n";
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    const std::string query = "SELECT PASSWORD FROM USERS WHERE USERNAME = '" + user + "';";

    char* errMsg = nullptr;
    std::string storedPassword;

    int result = static_cast<int>(DatabaseResult::DATABASE_ERROR);

    auto callback = [](void* data, int argc, char** argv, char** colNames) -> int
        {
            std::string* storedPassword = static_cast<std::string*>(data);
            if (argc > 0 && argv[0])
            {
                *storedPassword = argv[0]; // Store the password from the database
            }

            return 0;
        };

    if (sqlite3_exec(this->_dataBase, query.c_str(), callback, &storedPassword, &errMsg) != SQLITE_OK)
    {
        std::cerr << "DataBase: [ERORR]: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    // After the query, check if the password matches
    if (!storedPassword.empty())
    {
        result = (storedPassword == password) ? static_cast<int>(DatabaseResult::PASSWORD_MATCH) : static_cast<int>(DatabaseResult::PASSWORD_DISMATCH);
    }

    else
    {
        result = static_cast<int>(DatabaseResult::USER_NOT_FOUND);
    }

    return result;
}

int SqliteDataBase::addNewUser(const std::string& user, const std::string& password, const std::string& email)
{
    if (!this->_dataBase)
    {
        std::cerr << "DataBase: [ERROR]: Database isn't open!\n";
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    if (user.empty() || password.empty() || email.empty())
    {
        std::cerr << "DataBase: [ERROR]: Empty user, password, or email provided!\n";
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    int userExists = doesUserExist(user);

    if (userExists == static_cast<int>(DatabaseResult::USER_EXISTS))
    {
        return static_cast<int>(DatabaseResult::USER_EXISTS);
    }

    else if (userExists == static_cast<int>(DatabaseResult::DATABASE_ERROR))
    {
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    char* errMessage = nullptr;

    if (sqlite3_exec(this->_dataBase, "BEGIN TRANSACTION;", nullptr, nullptr, &errMessage) != SQLITE_OK)
    {
        sqlite3_free(errMessage);
        std::cerr << "DataBase: [ERROR]: Failed to begin transaction\n";
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    const std::string sql = "INSERT INTO USERS (USERNAME, PASSWORD, EMAIL) VALUES ('" + user + "', '" + password + "', '" + email + "');";

    if (executeQuery(sql) == static_cast<int>(DatabaseResult::DATABASE_ERROR))
    {
        std::cerr << "DataBase: [ERROR]: Failed to add user\n";
        sqlite3_exec(this->_dataBase, "ROLLBACK;", nullptr, nullptr, nullptr);
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    if (sqlite3_exec(this->_dataBase, "COMMIT;", nullptr, nullptr, &errMessage) != SQLITE_OK)
    {
        sqlite3_free(errMessage);
        std::cerr << "DataBase: [ERROR]: Failed to commit transaction\n";
        sqlite3_exec(this->_dataBase, "ROLLBACK;", nullptr, nullptr, nullptr);
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    return static_cast<int>(DatabaseResult::DATABASE_SUCCESS);
}

std::list<Question> SqliteDataBase::getQuestions(const int questionsAmount)
{
    int totalQuestions = getAmountOfQuestions();

    if (totalQuestions < questionsAmount)
    {
        int toFetch = questionsAmount - totalQuestions;

        addQuestionsFromOpenTDB(toFetch);
        totalQuestions = getAmountOfQuestions();
    }

    int validQuestionsAmount = min(questionsAmount, totalQuestions);

    std::string getQuestionsQuery = "SELECT * FROM QUESTIONS ORDER BY RANDOM() LIMIT " + std::to_string(validQuestionsAmount) + ";";

    std::list<Question> questions;
    char* errMsg = nullptr;

    if (sqlite3_exec(this->_dataBase, getQuestionsQuery.c_str(), processQuestionsCallback, &questions, &errMsg) != SQLITE_OK)
    {
        std::cerr << "[DATABASE]: [ERROR]: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return {};
    }

    return questions;
}

float SqliteDataBase::getPlayerAverageAnswerTime(const std::string& username)
{
    if (!isDataBaseOpen())
    {
        std::cerr << "[DATABASE]: ERROR: Database not open!" << std::endl;
        return static_cast<float>(DatabaseResult::DATABASE_ERROR);
    }

    const std::string GET_AVG_TIME_QUERY = "SELECT AVG_ANSWER_TIME FROM STATISTICS WHERE USERNAME = '" + username + "';";
    float averageAnswerTime = static_cast<float>(DatabaseResult::DATABASE_ERROR);

    auto callback = [](void* data, int argc, char** argv, char** colNames) -> int
    {
        if (argc > 0 && argv[0] != nullptr)
        {
            *static_cast<float*>(data) = std::stof(argv[0]);
        }

        return 0;
    };

    char* errMsg = nullptr;
    if (sqlite3_exec(this->_dataBase, GET_AVG_TIME_QUERY.c_str(), callback, &averageAnswerTime, &errMsg) != SQLITE_OK)
    {
        std::cerr << "[DATABASE]: ERROR: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return static_cast<float>(DatabaseResult::DATABASE_ERROR);
    }

    return averageAnswerTime;
}

int SqliteDataBase::getNumOfCorrectAnswers(const std::string& username)
{
    if (!isDataBaseOpen())
    {
        std::cerr << "DataBase: [ERROR]: Database not open!" << std::endl;
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    const std::string CORRECT_ANSWERS_QUERY = "SELECT CORRECT_ANSWERS FROM STATISTICS WHERE USERNAME = '" + username + "';";
    int correctAnswers = 0;

    auto callback = [](void* data, int argc, char** argv, char** colNames) -> int
    {
        *static_cast<int*>(data) = std::stoi(argv[0]);
        return SUCCESS;
    };

    char* errMsg = nullptr;
    if (sqlite3_exec(this->_dataBase, CORRECT_ANSWERS_QUERY.c_str(), callback, &correctAnswers, &errMsg) != SQLITE_OK)
    {
        std::cerr << "DataBase: [ERROR]: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    return correctAnswers;
}

int SqliteDataBase::getNumOfTotalAnswers(const std::string& username)
{
    if (!isDataBaseOpen())
    {
        std::cerr << "DataBase: [ERROR]: Database not open!" << std::endl;
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    const std::string TOTAL_ANSWERS_QUERY = "SELECT TOTAL_ANSWERS FROM STATISTICS WHERE USERNAME = '" + username + "';";
    int correctAnswers = 0;

    auto callback = [](void* data, int argc, char** argv, char** colNames) -> int
    {
        *static_cast<int*>(data) = std::stoi(argv[0]);
        return SUCCESS;
    };

    char* errMsg = nullptr;
    if (sqlite3_exec(this->_dataBase, TOTAL_ANSWERS_QUERY.c_str(), callback, &correctAnswers, &errMsg) != SQLITE_OK)
    {
        std::cerr << "DataBase: [ERROR]: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    return correctAnswers;
}

int SqliteDataBase::getNumOfPlayerGames(const std::string& username)
{
    if (!isDataBaseOpen())
    {
        std::cerr << "DataBase: [ERROR]: Database not open!" << std::endl;
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    const std::string GAMES_PLAYED_QUERY = "SELECT GAMES_PLAYED FROM STATISTICS WHERE USERNAME = '" + username + "';";
    int correctAnswers = 0;

    auto callback = [](void* data, int argc, char** argv, char** colNames) -> int
    {
        *static_cast<int*>(data) = std::stoi(argv[0]);
        return SUCCESS;
    };

    char* errMsg = nullptr;

    if (sqlite3_exec(this->_dataBase, GAMES_PLAYED_QUERY.c_str(), callback, &correctAnswers, &errMsg) != SQLITE_OK)
    {
        std::cerr << "DataBase: [ERROR]: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    return correctAnswers;
}

int SqliteDataBase::getPlayerScore(const std::string& username)
{    
    if (!isDataBaseOpen())
    {
        std::cerr << "DataBase: [ERROR]: Database not open!" << std::endl;
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    int correctAnswers = getNumOfCorrectAnswers(username), totalAnswers = getNumOfTotalAnswers(username), gamesPlayed = getNumOfPlayerGames(username);
    int wrongAnswers = totalAnswers - correctAnswers;
    float avgAnswerTime = getPlayerAverageAnswerTime(username);

    if (correctAnswers == static_cast<int>(DatabaseResult::DATABASE_ERROR) || totalAnswers == static_cast<int>(DatabaseResult::DATABASE_ERROR) || avgAnswerTime < 0)
    {
        std::cerr << "DataBase: [ERROR]: Invalid data for user: " << username << std::endl;
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    const int SCORE_FACTOR_POINTS = 10;

    // Avoid division by zero
    double safeWrongAnswers = max(1.0, static_cast<double>(wrongAnswers));
    double safeAvgTime = max(1.0, static_cast<double>(avgAnswerTime));

    double rawScore = (totalAnswers + gamesPlayed) / (safeWrongAnswers * safeAvgTime);
    int score = static_cast<int>(std::round(rawScore * SCORE_FACTOR_POINTS));

    // Ensure minimum score of 1 if user participated
    score = (totalAnswers > 0) ? max(1, score) : 0;

    return score;
}

int SqliteDataBase::submitGameStatistics(const std::string& username, const GameData& data)
{
    if (!isDataBaseOpen())
    {
        std::cerr << "DataBase: [ERROR]: Database not open!" << std::endl;
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    if (sqlite3_exec(_dataBase, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr) != SQLITE_OK)
    {
        std::cerr << "DataBase: [ERROR]: Failed to begin transaction\n";
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    char* errMsg = nullptr;
    bool userExists = doesUserExist(username);

    const std::string desiredQuery = userExists ? buildUpdateStatsQuery(username, data) : buildInsertStatsQuery(username, data);

    if ((sqlite3_exec(_dataBase, desiredQuery.c_str(), nullptr, nullptr, &errMsg) == SQLITE_OK) &&
        (sqlite3_exec(_dataBase, "COMMIT;", nullptr, nullptr, &errMsg) == SQLITE_OK))
    {
        return static_cast<int>(DatabaseResult::DATABASE_SUCCESS);
    }

    std::cerr << "DataBase: [ERROR]: " << (errMsg ? errMsg : "Unknown error") << std::endl;
    if (errMsg)
    {
        sqlite3_free(errMsg);
    }

    sqlite3_exec(_dataBase, "ROLLBACK;", nullptr, nullptr, nullptr);
    return static_cast<int>(DatabaseResult::DATABASE_ERROR);
}

std::vector<std::string> SqliteDataBase::getHighScores()
{
    std::vector<std::pair<std::string, int>> playersScores;

    if (!isDataBaseOpen())
    {
        std::cerr << "DataBase: [ERROR]: Database is not open!" << std::endl;
        return {};
    }

    const std::string GET_SCORES_QUERY = "SELECT USERNAME FROM STATISTICS;";
    char* errMsg = nullptr;

    scoresEntity scoresSavingEntity = { this, &playersScores };

    if (sqlite3_exec(_dataBase, GET_SCORES_QUERY.c_str(), processScoresCallback, &scoresSavingEntity, &errMsg) != SQLITE_OK)
    {
        std::cerr << "DataBase: [ERROR]: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return {};
    }

    if (playersScores.empty())
    {
        std::cerr << "DataBase: [ERROR]: No players found in the database." << std::endl;
        return {};
    }

    sortHighestScores(playersScores);

    const int TOP_PLAYERS_AMOUNT = 5;
    std::vector<std::string> highScores;

    for (int i = 0; i < min(TOP_PLAYERS_AMOUNT, static_cast<int>(playersScores.size())); i++)
    {
        highScores.push_back(playersScores[i].first);
    }

    return highScores;
}


int SqliteDataBase::executeQuery(const std::string& executedSQLQuery)
{
    char* errMsg = nullptr;

    if (sqlite3_exec(this->_dataBase, executedSQLQuery.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cerr << "DataBase: [ERROR]: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    return static_cast<int>(DatabaseResult::DATABASE_SUCCESS);
}

void SqliteDataBase::initializeTriviaDB()
{
    std::cout << "Initializing database...\n";

    // Database tables creation queries - 
    const char* CREATE_USERS = "CREATE TABLE IF NOT EXISTS \"USERS\" ("
        "\"USERNAME\" TEXT PRIMARY KEY, "
        "\"PASSWORD\" TEXT NOT NULL, "
        "\"EMAIL\" TEXT NOT NULL UNIQUE); ";

    const char* CREATE_QUESTIONS = "CREATE TABLE IF NOT EXISTS \"QUESTIONS\" ("
        "\"ID\" INTEGER PRIMARY KEY AUTOINCREMENT, "
        "\"QUESTION\" TEXT NOT NULL, "
        "CORRECT_ANSWER TEXT NOT NULL, "
        "WRONG_ANSWER1 TEXT NOT NULL, "
        "WRONG_ANSWER2 TEXT NOT NULL, "
        "WRONG_ANSWER3 TEXT NOT NULL); ";

    const char* CREATE_STATISTICS = "CREATE TABLE IF NOT EXISTS \"STATISTICS\" ("
        "\"ID\" INTEGER PRIMARY KEY AUTOINCREMENT, "
        "\"USERNAME\" TEXT NOT NULL UNIQUE, "
        "\"CORRECT_ANSWERS\" INTEGER NOT NULL DEFAULT 0, "
        "\"TOTAL_ANSWERS\" INTEGER NOT NULL DEFAULT 0, "
        "\"AVG_ANSWER_TIME\" REAL NOT NULL DEFAULT 0.0, "
        "\"GAMES_PLAYED\" INTEGER NOT NULL DEFAULT 0, "
        "FOREIGN KEY (\"USERNAME\") REFERENCES USERS(\"USERNAME\"));";

    char* errMessage = nullptr;

    int result = sqlite3_exec(this->_dataBase, "BEGIN TRANSACTION;", nullptr, nullptr, &errMessage);

    if (result != SQLITE_OK)
    {
        std::string errorMsg(errMessage);
        sqlite3_free(errMessage);
        throw std::runtime_error("Error starting transaction: " + errorMsg + "\n");
    }

    try
    {
        executeQuery(CREATE_USERS);
        executeQuery(CREATE_QUESTIONS);
        executeQuery(CREATE_STATISTICS);

        const std::string COMMIT_QUERY = "COMMIT;";
        executeQuery(COMMIT_QUERY.c_str());

        std::cout << "Trivia DataBase Table created successfully!" << std::endl;
    }

    catch (const std::runtime_error& e)
    {
        const int FAILURE = 1;

        std::cout << e.what() << std::endl;
        sqlite3_exec(this->_dataBase, "ROLLBACK;", nullptr, nullptr, nullptr);
        exit(FAILURE); // If we still couldn't create the database we can not run the trivia...
    }
}

bool SqliteDataBase::isDataBaseOpen()
{
    return (this->_dataBase != nullptr);
}

void SqliteDataBase::sortHighestScores(std::vector<std::pair<std::string, int>>& playersScores)
{
    std::sort(playersScores.begin(), playersScores.end(), [](const std::pair<std::string, int>& currentPlayer, const std::pair<std::string, int>& otherPlayer)
    {
        return currentPlayer.second > otherPlayer.second;
    });
}

int SqliteDataBase::getAmountOfQuestions()
{
    const std::string COUNT_QUERY = "SELECT COUNT(*) FROM QUESTIONS;";
    const int SUCCESS = 0;

    int totalQuestions = 0;

    char* errMsg = nullptr;

    auto countCallback = [](void* data, int argc, char** argv, char** colNames) -> int
    {
        if (argc > 0 && argv[0] != nullptr)
        {
            *static_cast<int*>(data) = std::stoi(argv[0]);  // Set the count of questions
        }

        return SUCCESS;
    };

    if (sqlite3_exec(this->_dataBase, COUNT_QUERY.c_str(), countCallback, &totalQuestions, &errMsg) != SQLITE_OK)
    {
        std::cerr << "[ERROR] SQLite: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    return totalQuestions;
}

std::string SqliteDataBase::buildUpdateStatsQuery(const std::string& username, const GameData& data)
{
    return "UPDATE STATISTICS SET "
        "CORRECT_ANSWERS = CORRECT_ANSWERS + " + std::to_string(data.correctAnswerCount) + ", "
        "TOTAL_ANSWERS = TOTAL_ANSWERS + " + std::to_string(data.correctAnswerCount + data.wrongAnswerCount) + ", "
        "AVG_ANSWER_TIME = ((AVG_ANSWER_TIME * GAMES_PLAYED) + " + std::to_string(data.averageAnswerTime) + ") / (GAMES_PLAYED + 1), "
        "GAMES_PLAYED = GAMES_PLAYED + 1 "
        "WHERE USERNAME = '" + username + "';";
}

std::string SqliteDataBase::buildInsertStatsQuery(const std::string& username, const GameData& data)
{
    return "INSERT INTO STATISTICS (USERNAME, CORRECT_ANSWERS, TOTAL_ANSWERS, AVG_ANSWER_TIME, GAMES_PLAYED) VALUES ('" +
        username + "', " + std::to_string(data.correctAnswerCount) + ", " +
        std::to_string(data.correctAnswerCount + data.wrongAnswerCount) + ", " +
        std::to_string(data.averageAnswerTime) + ", 1);";
}

int SqliteDataBase::processScoresCallback(void* data, int argc, char** argv, char** colNames)
{
    const int SUCCESS = 0;

    // Calling callback with both this* and vector of scores so we can use GetPlayerScore() method in callback
    auto* scoresCallbackContentPtr = static_cast<scoresEntity*>(data);
    SqliteDataBase* db = scoresCallbackContentPtr->db;
    auto* scores = scoresCallbackContentPtr->scores;

    if (argv[0] != nullptr)
    {
        std::string username = argv[0];
        int score = db->getPlayerScore(username);

        scores->push_back({ username, score });
    }

    return SUCCESS;
}

int SqliteDataBase::processQuestionsCallback(void* data, int argc, char** argv, char** colNames)
{
    if (argc > 0)
    {
        const int QUESTION_TEXT_INDEX = 1, CORRECT_ANSWER_INDEX = 2, WRONG_ANSWER1_INDEX = 3, WRONG_ANSWER2_INDEX = 4, WRONG_ANSWER3_INDEX = 5;
        std::string questionText = argv[QUESTION_TEXT_INDEX];
        std::string correctAnswer = argv[CORRECT_ANSWER_INDEX];

        std::vector<std::string> possibleAnswers;
        possibleAnswers.push_back(argv[CORRECT_ANSWER_INDEX]);
        possibleAnswers.push_back(argv[WRONG_ANSWER1_INDEX]);
        possibleAnswers.push_back(argv[WRONG_ANSWER2_INDEX]);
        possibleAnswers.push_back(argv[WRONG_ANSWER3_INDEX]);

        std::random_device rd;
        std::shuffle(possibleAnswers.begin(), possibleAnswers.end(), rd);

        auto correctAnswerIdIt = std::find(possibleAnswers.cbegin(), possibleAnswers.cend(), correctAnswer);

        if (correctAnswerIdIt != possibleAnswers.cend())
        {
            int correctAnswerId = static_cast<int>(std::distance(possibleAnswers.cbegin(), correctAnswerIdIt));

            // Creating the question based on the extracted data and adding it to the lists of questions
            Question question(questionText, possibleAnswers, correctAnswerId);
            static_cast<std::list<Question>*>(data)->push_back(question);
        }

        else
        {
            std::cerr << "Database: [ERROR]: Correct answer not found for question: " << questionText << std::endl;
            return FAILURE;
        }
    }

    return SUCCESS;
}

// Inserting random questions Methods - (V2)
void SqliteDataBase::addQuestionsFromOpenTDB(const int questionsAmount)
{
    CURL* curl;
    CURLcode res;

    std::string responseData;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) 
    {
        std::string url = "https://opentdb.com/api.php?type=multiple&amount=" + std::to_string(questionsAmount);

        // Setting the desired URL, the callback method and the buffer container for the returned response of HTTP Get Request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeGETResponse);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            std::cerr << "[DATABASE]: ERROR: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    try
    {
        auto jsonResponse = nlohmann::json::parse(responseData)["results"];

        std::cout << "[DATABASE]: Successfully parsed " << jsonResponse.size() << " questions." << std::endl;
        insertQuestionsIntoDataBase(jsonResponse);
    }

    catch (const nlohmann::json::parse_error& e)
    {
        std::cerr << "[DATABASE]: JSON parse error: " << e.what() << std::endl;
        std::cerr << "Raw response:\n" << responseData << std::endl;
    }

    catch (const std::exception& e)
    {
        std::cerr << "[DATABASE]: " << e.what() << std::endl;
    }
    
}

void SqliteDataBase::insertQuestionsIntoDataBase(const nlohmann::json& jsonResponse)
{
    const int WRONG_ANSWERS_AMOUNT = 3;
    const int MAX_QUESTIONS = 10;

    int questionCount = 0;

    std::cout << "[DATABASE]: Inserting 10 random questions from OpenTDB Website" << std::endl;
    for (const auto& currentQuestionJSON : jsonResponse)
    {
        if (questionCount >= MAX_QUESTIONS)
        {
            std::cout << "[DATABASE]: Reached max number of questions to insert (10)." << std::endl;
            break;  // Exit the loop after inserting 10 questions
        }

        // Extracting each question's fields in order to construct the SQL Query and insert the question into database
        std::string question = decodeHtmlCharacters(currentQuestionJSON.at("question").get<std::string>());
        std::string correctAnswer = decodeHtmlCharacters(currentQuestionJSON.at("correct_answer").get<std::string>());
        std::vector<std::string> incorrectAnswers = currentQuestionJSON.at("incorrect_answers").get<std::vector<std::string>>();

        prepareSQLQuery(question);
        prepareSQLQuery(correctAnswer);
        // Decoding each answer in the returned vector (some of them may have HTML special characters)
        for (auto& ans : incorrectAnswers)
        {
            ans = decodeHtmlCharacters(ans);
            prepareSQLQuery(ans);
        }

        std::string currentQuestionQuery = "INSERT INTO QUESTIONS (QUESTION, CORRECT_ANSWER, WRONG_ANSWER1, WRONG_ANSWER2, WRONG_ANSWER3) VALUES ('" +
            question + "', '" + correctAnswer + "', '" +
            incorrectAnswers[0] + "', '" +
            incorrectAnswers[1] + "', '" +
            incorrectAnswers[2] + "');";

        if (executeQuery(currentQuestionQuery) != static_cast<int>(DatabaseResult::DATABASE_SUCCESS))
        {
            std::cerr << "[DATABASE]: Failed to insert question: " << question << std::endl;
            questionCount--;
        }

        questionCount++;
    }
}

size_t SqliteDataBase::writeGETResponse(void* ptr, size_t size, size_t nmemb, void* data)
{
    // Extracting the total size of the each chunk, then adding it to the full response
    size_t total_size = size * nmemb;

    // Saving all the returned response in the given string arguement
    static_cast<std::string*>(data)->append(static_cast<char*>(ptr), total_size);

    return total_size;
}

std::string SqliteDataBase::decodeHtmlCharacters(const std::string& input)
{
    std::unordered_map<std::string, std::string> HtmlCharacters =
    {
        {"&amp;", "&"},
        {"&lt;", "<"},
        {"&gt;", ">"},
        {"&quot;", "\""},
        {"&apos;", "\'"},
        {"&#039;", "\'"},
        {"&nbsp;", " "},
    };

    std::string result = input;

    for (const auto& entity : HtmlCharacters)
    {
        size_t pos = 0;

        // We replace in the result string the HTML special characters for each special character
        while ((pos = result.find(entity.first, pos)) != std::string::npos) 
        {
            result.replace(pos, entity.first.length(), entity.second);

            pos += entity.second.length();
        }
    }
    
    return result;
}

void SqliteDataBase::prepareSQLQuery(std::string& input)
{
    // Removing any problematic characters which may cause errors with SQL query
    input.erase(std::remove(input.begin(), input.end(), '"'), input.end());
    input.erase(std::remove(input.begin(), input.end(), '\''), input.end());
    input.erase(std::remove(input.begin(), input.end(), ';'), input.end());
}

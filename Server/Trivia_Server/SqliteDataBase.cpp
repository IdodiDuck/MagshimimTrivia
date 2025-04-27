#include "SqliteDataBase.h"

#include <io.h>
#include <iostream>
#include <unordered_map>

#include <curl/curl.h> // Used for processing HTTP Responses

SqliteDataBase::SqliteDataBase(): _dataBaseName("trivia.db"), _dataBase(nullptr)
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
    int result = sqlite3_open(this->_dataBaseName.c_str(), &this->_dataBase); // database pointer is referencing now to the database file

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

    // If the original database did not exist we will initialize it's tables right after we created it by sqlite3_open
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

    // Callback function for finding the the existance of the user
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

    // Returning if the user exists or not based on the callback extraction results
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

    // Callback function to process the result
    auto callback = [](void* data, int argc, char** argv, char** colNames) -> int 
    {
        std::string* storedPassword = static_cast<std::string*>(data);
        if (argc > 0 && argv[0]) 
        {
            *storedPassword = argv[0]; // Store the password from the database
        }

        return 0;
    };

    // Execute the query and process the result using the callback
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

    // Check if the user exists first
    int userExists = doesUserExist(user);

    if (userExists == static_cast<int>(DatabaseResult::USER_EXISTS))
    {
        return static_cast<int>(DatabaseResult::USER_EXISTS);
    }

    // Check if an error occurred when we checked if the user exists
    else if (userExists == static_cast<int>(DatabaseResult::DATABASE_ERROR))
    {
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    char* errMessage = nullptr;

    // Begin the transaction
    if (sqlite3_exec(this->_dataBase, "BEGIN TRANSACTION;", nullptr, nullptr, &errMessage) != SQLITE_OK)
    {
        sqlite3_free(errMessage);
        std::cerr << "DataBase: [ERROR]: Failed to begin transaction\n";
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    const std::string sql = "INSERT INTO USERS (USERNAME, PASSWORD, EMAIL) VALUES ('" + user + "', '" + password + "', '" + email + "');";

    // Execute the insertion query
    if (executeQuery(sql) == static_cast<int>(DatabaseResult::DATABASE_ERROR))
    {
        std::cerr << "DataBase: [ERROR]: Failed to add user\n";
        sqlite3_exec(this->_dataBase, "ROLLBACK;", nullptr, nullptr, nullptr);
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    // Commit the transaction
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
    return std::list<Question>();
}

float SqliteDataBase::getPlayerAverageAnswerTime(const std::string& username)
{
    return 0.0f;
}

int SqliteDataBase::getNumOfCorrectAnswers(const std::string& username)
{
    if (!isDataBaseOpen())
    {
        std::cerr << "[ERROR] Database not open!" << std::endl;
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);;
    }

    const std::string CORRECT_ANSWERS_QUERY = "SELECT CORRECT_ANSWERS FROM STATISTICS WHERE USERNAME = '" + username + "';";
    int correctAnswers = 0;

    auto callback = [](void* data, int argc, char** argv, char** colNames) -> int
    {
        *static_cast<int*>(data) = std::stoi(argv[0]);
        return 0; // Success code
    };

    char* errMsg = nullptr;
    if (sqlite3_exec(this->_dataBase, CORRECT_ANSWERS_QUERY.c_str(), callback, &correctAnswers, &errMsg) != SQLITE_OK)
    {
        std::cerr << "[ERROR] SQLite: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);;
    }

    return correctAnswers;
}

int SqliteDataBase::getNumOfTotalAnswers(const std::string& username)
{
    if (!isDataBaseOpen())
    {
        std::cerr << "[ERROR] Database not open!" << std::endl;
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);;
    }

    const std::string TOTAL_ANSWERS_QUERY = "SELECT TOTAL_ANSWERS FROM STATISTICS WHERE USERNAME = '" + username + "';";
    int correctAnswers = 0;

    auto callback = [](void* data, int argc, char** argv, char** colNames) -> int
    {
        *static_cast<int*>(data) = std::stoi(argv[0]);
        return 0; // Success code
    };

    char* errMsg = nullptr;
    if (sqlite3_exec(this->_dataBase, TOTAL_ANSWERS_QUERY.c_str(), callback, &correctAnswers, &errMsg) != SQLITE_OK)
    {
        std::cerr << "[ERROR] SQLite: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);;
    }

    return correctAnswers;
}

int SqliteDataBase::getNumOfPlayerGames(const std::string& username)
{
    if (!isDataBaseOpen())
    {
        std::cerr << "[ERROR] Database not open!" << std::endl;
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);;
    }

    const std::string GAMES_PLAYED_QUERY = "SELECT GAMES_PLAYED FROM STATISTICS WHERE USERNAME = '" + username + "';";
    int correctAnswers = 0;

    auto callback = [](void* data, int argc, char** argv, char** colNames) -> int
    {
        *static_cast<int*>(data) = std::stoi(argv[0]);
        return 0; // Success code
    };

    char* errMsg = nullptr;
    if (sqlite3_exec(this->_dataBase, GAMES_PLAYED_QUERY.c_str(), callback, &correctAnswers, &errMsg) != SQLITE_OK)
    {
        std::cerr << "[ERROR] SQLite: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);;
    }

    return correctAnswers;
}

int SqliteDataBase::getPlayerScore(const std::string& username)
{
    const int CORRECT_ANSWERS_POINTS = 10, ANSWER_TIME_PENALTY_POINTS = 2;
    
    if (!isDataBaseOpen())
    {
        std::cerr << "[ERROR] Database not open!" << std::endl;
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    int correctAnswers = getNumOfCorrectAnswers(username);
    int totalAnswers = getNumOfTotalAnswers(username);
    float avgAnswerTime = getPlayerAverageAnswerTime(username);

    if (correctAnswers == static_cast<int>(DatabaseResult::DATABASE_ERROR) || totalAnswers == static_cast<int>(DatabaseResult::DATABASE_ERROR) || avgAnswerTime < 0)
    {
        std::cerr << "[ERROR] Invalid data for user: " << username << std::endl;
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    // Calculating the total score by the amount of correct answers
    int score = correctAnswers * CORRECT_ANSWERS_POINTS;

    // Deducting from the score depends on the time it took for the user to answer
    score -= static_cast<int>(avgAnswerTime * ANSWER_TIME_PENALTY_POINTS);

    score = max(0, score); // Negative amount of points is considered as zero
    return score;
}

std::vector<std::string> SqliteDataBase::getHighScores()
{
    std::vector<std::pair<std::string, int>> playersScores; // vector of pairs of username and his score

    if (!isDataBaseOpen())
    {
        std::cerr << "[ERROR] Database not open!" << std::endl;
        return std::vector<std::string>();
    }

    const std::string GET_SCORES_QUERY = "SELECT USERNAME FROM STATISTICS;";

    char* errMsg = nullptr;

    // Passing this pointer as data to the callback
    if (sqlite3_exec(_dataBase, GET_SCORES_QUERY.c_str(), processScoresCallback, this, &errMsg) != SQLITE_OK)
    {
        std::cerr << "[ERROR] SQLite: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return std::vector<std::string>(); // Return empty vector if an error occurred with database
    }

    if (playersScores.empty())
    {
        std::cerr << "[INFO] No players found in the database." << std::endl;
        return std::vector<std::string>(); // Return empty vector if there're no players
    }
    
    sortHighestScores(playersScores);

    const int TOP_PLAYERS_AMOUNT = 5;

    std::vector<std::string> highScores;
    for (int currentPlayer = 0; currentPlayer < min(TOP_PLAYERS_AMOUNT, static_cast<int>(playersScores.size())); currentPlayer++)
    {
        highScores.push_back(playersScores[currentPlayer].first); // Pushing top 5 usernames (Or less if we don't have 5 yet)
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
        sqlite3_free(errMessage);  // Free the error message memory
        throw std::runtime_error("Error starting transaction: " + errorMsg + "\n");
    }

    try
    {
        // Trying to create all database tables
        executeQuery(CREATE_USERS);
        executeQuery(CREATE_QUESTIONS);
        executeQuery(CREATE_STATISTICS);

        // Commit the transaction if the query was successful
        const std::string COMMIT_QUERY = "COMMIT;";
        executeQuery(COMMIT_QUERY.c_str());

        std::cout << "Trivia DataBase Table created successfully!" << std::endl;
    }

    catch (const std::runtime_error& e)
    {
        const int FAILURE = 1;

        // Rollback if any exception occurs
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
    // Sort players by score in descending order
    std::sort(playersScores.begin(), playersScores.end(), [](const std::pair<std::string, int>& currentPlayer, const std::pair<std::string, int>& otherPlayer)
    {
        return currentPlayer.second > otherPlayer.second; // Sorting by the score
    });
}

int SqliteDataBase::processScoresCallback(void* data, int argc, char** argv, char** colNames)
{
    SqliteDataBase* dbInstance = static_cast<SqliteDataBase*>(data);

    if (argv[0] != nullptr) 
    {
        std::vector<std::pair<std::string, int>>* scores = static_cast<std::vector<std::pair<std::string, int>>*>(data);
        std::string username = argv[0];

        int score = dbInstance->getPlayerScore(username);
        scores->push_back({ username, score });
    }
    return 0; // Success code
}

// Inserting 10 random questions Methods - (V2)
void SqliteDataBase::addQuestionsFromOpenTDB()
{
    CURL* curl;
    CURLcode res;

    std::string responseData;

    // Initializing the global libcurl environment
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    // Checking if the CURL handle was created successfully
    if (curl) 
    {
        std::string url = "https://opentdb.com/api.php?amount=10&type=multiple";

        // Setting the desired URL, the callback method and the buffer container for the returned response of HTTP Get Request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeGETResponse);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

        // Sending HTTP Request
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            std::cerr << "[DATABASE]: ERROR: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl); // Cleanup CURL handle allocated memory
    }

    // Cleanup CURL global environment
    curl_global_cleanup();

    try
    {
        auto jsonResponse = nlohmann::json::parse(responseData)["results"];

        std::cout << "[DATABASE]: Successfully parsed " << jsonResponse.size() << " questions." << std::endl;
        insertQuestionsIntoDataBase(jsonResponse);
    }

    // Catching JSON Errors
    catch (const nlohmann::json::parse_error& e)
    {
        std::cerr << "[DATABASE]: JSON parse error: " << e.what() << std::endl;
        std::cerr << "Raw response:\n" << responseData << std::endl;
    }

    // Catching Database Errors
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

        // Building the SQL Insertion query for the current question with the given answers
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

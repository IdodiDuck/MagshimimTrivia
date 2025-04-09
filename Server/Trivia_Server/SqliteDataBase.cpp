#include "SqliteDataBase.h"

#include <io.h>
#include <iostream>

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
            std::cerr << "Error during database initialization: " << e.what() << std::endl;
            close();
            return false;
        }
    }

    std::cout << "Database opened successfully!" << std::endl;
    return true;
}

bool SqliteDataBase::close()
{
    std::cout << "DATABASE: Attempting to close database..." << std::endl;
    if (!isDataBaseOpen())
    {
        std::cerr << "Error: Trivia DataBase is not currently open!\n";
        return false;
    }

    int result = sqlite3_close(this->_dataBase);

    if (result != SQLITE_OK)
    {
        std::cerr << "Error: Failed to close database: " << sqlite3_errmsg(this->_dataBase) << "\n";
        this->_dataBase = nullptr;
        return false;
    }

    this->_dataBase = nullptr;
    std::cout << "Database closed successfully!" << std::endl;
    return true;
}

int SqliteDataBase::doesUserExist(const std::string& user)
{
    if (!isDataBaseOpen())
    {
        std::cerr << "Error: Database isn't open!" << std::endl;
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
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    // Returning if the user exists or not based on the callback extraction results
    return (userExists == static_cast<int>(DatabaseResult::USER_EXISTS)) ? userExists : static_cast<int>(DatabaseResult::USER_NOT_FOUND);
}

int SqliteDataBase::doesPasswordMatch(const std::string& user, const std::string& password)
{
    if (!(isDataBaseOpen))
    {
        std::cerr << "Error: Database isn't open!" << std::endl;
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    if ((user.empty()) || (password.empty()))
    {
        std::cerr << "Error: Empty username or password provided!\n";
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
        std::cerr << "SQL error: " << errMsg << std::endl;
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
        std::cerr << "Error: Database isn't open!\n";
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    if (user.empty() || password.empty() || email.empty())
    {
        std::cerr << "Error: Empty user, password, or email provided!\n";
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
        std::cerr << "Error: Failed to begin transaction\n";
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    const std::string sql = "INSERT INTO USERS (USERNAME, PASSWORD, EMAIL) VALUES ('" + user + "', '" + password + "', '" + email + "');";

    // Execute the insertion query
    if (executeQuery(sql) == static_cast<int>(DatabaseResult::DATABASE_ERROR))
    {
        std::cerr << "Error: Failed to add user\n";
        sqlite3_exec(this->_dataBase, "ROLLBACK;", nullptr, nullptr, nullptr);
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    // Commit the transaction
    if (sqlite3_exec(this->_dataBase, "COMMIT;", nullptr, nullptr, &errMessage) != SQLITE_OK)
    {
        sqlite3_free(errMessage);
        std::cerr << "Error: Failed to commit transaction\n";
        sqlite3_exec(this->_dataBase, "ROLLBACK;", nullptr, nullptr, nullptr);
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    return static_cast<int>(DatabaseResult::DATABASE_SUCCESS);
}

int SqliteDataBase::executeQuery(const std::string& executedSQLQuery)
{
    char* errMsg = nullptr;

    if (sqlite3_exec(this->_dataBase, executedSQLQuery.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return static_cast<int>(DatabaseResult::DATABASE_ERROR);
    }

    return static_cast<int>(DatabaseResult::DATABASE_SUCCESS);
}

void SqliteDataBase::initializeTriviaDB()
{
    std::cout << "Initializing database...\n";

    const char* CREATE_USERS = "CREATE TABLE IF NOT EXISTS \"USERS\" ("
        "\"USERNAME\" TEXT PRIMARY KEY,"
        "\"PASSWORD\" TEXT NOT NULL,"
        "\"EMAIL\" TEXT NOT NULL UNIQUE"
        "); ";

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
        // Trying to create users database table
        executeQuery(CREATE_USERS);

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

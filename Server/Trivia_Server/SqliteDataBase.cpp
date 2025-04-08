#include "SqliteDataBase.h"

int executeQuery(const std::string& sql)
{
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) 
    {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return DATABASE_ERROR;
    }
    return 0;
}

void initializeTriviaDB()
{
    std::cout << "Initializing database...\n";

    const char* CREATE_USERS = "CREATE TABLE IF NOT EXISTS \"USERS\" ("
        "\"USERNAME\" TEXT PRIMARY KEY,"
        "\"PASSWORD\" TEXT NOT NULL,"
        "\"EMAIL\" TEXT NOT NULL UNIQUE"
        "); ";

    char* errMessage = nullptr;

    int result = sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMessage);

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

        // Commit the transaction if all queries were successful
        const std::string COMMIT_QUERY = "COMMIT;";
        executeQuery(COMMIT_QUERY.c_str());

        std::cout << "Trivia DataBase Table created successfully!" << std::endl;
    }

    catch (const std::runtime_error& e)
    {
        // Rollback if any exception occurs
        std::cout << e.what() << std::endl;
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        exit(1); // If we still couldn't create the database we can not run the trivia...
    }
}

bool SqliteDataBase::open()
{
    const int DATA_BASE_EXISTS_CODE = 0;

    if (db)
    {
        std::cerr << "Error: Trivia DataBase is already open!\n";
        return false;
    }

    int dbExists = _access(TRIVIA_DB.c_str(), DATA_BASE_EXISTS_CODE);
    int result = sqlite3_open(TRIVIA_DB.c_str(), &db); // galleryDB attribute pointer is changed

    if (result != SQLITE_OK)
    {
        std::cerr << "Error: Cannot open database: " + std::string(sqlite3_errmsg(db)) + "\n";
        return false;
    }

    const std::string ACTIVATE_FOREIGN_KEYS = "PRAGMA foreign_keys = ON;";

    // Activate foreign key constraints
    if (executeQuery(ACTIVATE_FOREIGN_KEYS.c_str()) == DATABASE_ERROR)
        return false;

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
    if (!db)
    {
        std::cerr << "Error: Trivia DataBase isnt currently open!\n";
        return false;
    }

    int result = sqlite3_close(db);

    if (result != SQLITE_OK)
    {
        std::cerr << "Error: Failed to close database: " << sqlite3_errmsg(db) << "\n";
        db = nullptr;
        return false;
    }

    db = nullptr;
    std::cout << "Database closed successfully!" << std::endl;
    return true;
}

int SqliteDataBase::doesUserExist(const std::string& user)
{
    std::string sql = "SELECT USERNAME FROM USERS WHERE USERNAME = '" + user + "';";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        return DATABASE_ERROR;
    }

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result == SQLITE_ROW)
    {
        return USER_EXISTS;
    }
    else if (result == SQLITE_DONE)
    {
        return USER_NOT_FOUND;
    }

    return DATABASE_ERROR;
}

int SqliteDataBase::doesPasswordMatch(const std::string& user, const std::string& password)
{
    if (!db)
    {
        std::cerr << "Error: Database isnt open!\n";
        return DATABASE_ERROR;
    }

    if (user.empty() || password.empty())
    {
        std::cerr << "Error: Empty username or password provided!\n";
        return DATABASE_ERROR;
    }

    const std::string query = "SELECT PASSWORD FROM USERS WHERE USERNAME = '" + user + "';";
    sqlite3_stmt* stmt = nullptr;
    int result = DATABASE_ERROR;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        return DATABASE_ERROR;
    }

    sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        std::string pass = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        result = (pass == password) ? PASSWORD_MATCH : PASSWORD_DISMATCH;
    }
    else
    {
        result = USER_NOT_FOUND;
    }

    sqlite3_finalize(stmt);
    return result;
}

int SqliteDataBase::addNewUser(const std::string& user, const std::string& password, const std::string& email)
{
    if (!db)
    {
        std::cerr << "Error: Database isnt open!\n";
        return DATABASE_ERROR;
    }

    if (user.empty() || password.empty() || email.empty())
    {
        std::cerr << "Error: Empty user, password or email provided!\n";
        return DATABASE_ERROR;
    }

    // Check if the user exists first
    int exists = doesUserExist(user);
    if (exists == USER_EXISTS)
    {
        return USER_EXISTS;
    }
    else if (exists == DATABASE_ERROR)
    {
        return DATABASE_ERROR;
    }

    char* errMessage = nullptr;

    if (sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMessage) != SQLITE_OK)
    {
        sqlite3_free(errMessage);  // Free the error message memory
        return DATABASE_ERROR;
    }

    int result = DATABASE_ERROR;
    const std::string sql = "INSERT INTO USERS (USERNAME, PASSWORD, EMAIL) VALUES ('" + user + "', '" + password + "', '" + email + "');";

    if (executeQuery(sql) == DATABASE_ERROR)
    {
        std::cerr << "Error: Failed to add user\n";
    }

    else
        result = USER_ADDED;

    if (executeQuery("COMMIT") == DATABASE_ERROR || result != USER_ADDED)
    {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return DATABASE_ERROR;
    }

    return result;
}

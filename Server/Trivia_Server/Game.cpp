#include "Game.h"

Game::Game(int gameId, std::vector<Question> questions, std::map<std::string, GameData> users)
    : m_gameId(gameId), m_questions(questions), m_players(users), m_totalQuestions((unsigned int)questions.size()), m_state(GameState::WAITING)
{
    if (!users.empty()) 
    {
        m_state = GameState::IN_PROGRESS;
    }
}


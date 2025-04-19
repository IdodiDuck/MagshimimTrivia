#include "Question.h"

Question::Question(const std::string& question, const std::vector<std::string>& possibleAnswers, const int correctAnswerId):
	m_question(question), m_possibleAnswers(possibleAnswers), m_correctAnswerId(correctAnswerId)
{

}

Question::~Question()
{
	this->m_possibleAnswers.clear();
}

std::string Question::getQuestion() const
{
	return this->m_question;
}

std::vector<std::string> Question::getPossibleAnswers() const
{
	return this->m_possibleAnswers;
}

int Question::getCorrectAnswerId() const
{
	return this->m_correctAnswerId;
}

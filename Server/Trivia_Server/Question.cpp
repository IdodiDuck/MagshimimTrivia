#include "Question.h"

Question::Question(const std::string& question, const std::vector<std::string>& possibleAnswers, const int correctAnswerId) :
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

std::string Question::getCorrectAnswer() const
{
	if (m_correctAnswerId >= 0 && m_correctAnswerId < m_possibleAnswers.size()) 
	{
		return m_possibleAnswers[m_correctAnswerId];
	}
	return "";
}

bool Question::operator==(const Question& other) const
{
	return (this->m_question == other.getQuestion());
}

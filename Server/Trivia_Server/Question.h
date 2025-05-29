#pragma once

#include <vector>
#include <string>

class Question
{
public:

	// C'tor & D'tor - 
	Question() = default;
	Question(const std::string& question, const std::vector<std::string>& possibleAnswers, const int correctAnswerId);
	~Question();

	// Getters - 
	std::string getQuestion() const;
	std::vector<std::string> getPossibleAnswers() const;
	int getCorrectAnswerId() const;
	std::string getCorrectAnswer() const;

	// Operators - 
	bool operator==(const Question& other) const;

private:
	// Attributes - 
	std::string m_question;
	std::vector<std::string> m_possibleAnswers;
	int m_correctAnswerId;

};


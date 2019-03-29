#pragma once

#include "IShowable.h"
#include "logic.h"
#include <bitset>
#include <functional>

class automata
{
public:
	typedef std::function<int(std::bitset<128U>&, bool)> heuristic;

	automata();

	automata(heuristic h);

	~automata();

	std::pair<logic::action, std::bitset<128>> getBestMove(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, unsigned int timeLeft);

private:
	heuristic h;
	sf::Clock clock;
	std::pair<logic::action, std::bitset<128>> alphaBeta(std::bitset<128U>& state, bool isBlack, unsigned int& moveLeft, unsigned int& timeLeft);
	std::pair<logic::action, std::bitset<128>> maxTop(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, unsigned int& timeLeft, int alpha, int beta);
	int maxValue(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, unsigned int& timeLeft, int alpha, int beta);
	int minValue(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, unsigned int& timeLeft, int alpha, int beta);
	bool terminateTest(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft);

	int basicHeuristic(std::bitset<128U>&, bool);
};


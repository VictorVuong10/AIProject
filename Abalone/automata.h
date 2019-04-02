#pragma once

#include "IShowable.h"
#include "logic.h"
#include <bitset>
#include <functional>
#include <queue>

class automata
{
public:
	typedef std::function<int(std::bitset<128U>&, bool)> heuristic;

	static const std::bitset<128U> MASKS_BLACK;
	static const std::bitset<128U> MASKS_WHITE;

	static constexpr unsigned int MIDDLE_H[61] = {
		   1, 1, 1, 1, 1,
		   1, 2, 2, 2, 2, 1,
		   1, 2, 3, 3, 3, 2, 1,
		   1, 2, 3, 4, 4, 3, 2, 1,
		   1, 2, 3, 4, 5, 4, 3, 2, 1,
		   1, 2, 3, 4, 4, 3, 2, 1,
		   1, 2, 3, 3, 3, 2, 1,
		   1, 2, 2, 2, 2, 1,
		   1, 1, 1, 1, 1,
	};

	automata();

	automata(heuristic h);

	~automata();

	std::pair<logic::action, std::bitset<128>> getBestMove(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, int timeLeft);

private:
	std::mutex blocker;
	std::mutex mtQ;
	std::mutex mtVal;
	int counter = 0;
	sf::Clock clock;
	heuristic h;
	std::pair<logic::action, std::bitset<128>> alphaBeta(std::bitset<128U>& state, bool isBlack, unsigned int& moveLeft, int& timeLeft);
	std::pair<std::pair<logic::action, std::bitset<128>>, int> maxTop(std::bitset<128U>& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int & timeLeft, int alpha, int beta);
	int maxValue(std::bitset<128U>& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int& timeLeft, int alpha, int beta);
	int minValue(std::bitset<128U>& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int& timeLeft, int alpha, int beta);
	bool terminateTest(std::bitset<128U>& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int & timeLeft);

	int basicHeuristic(std::bitset<128U>&, bool);
};


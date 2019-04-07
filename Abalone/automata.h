#pragma once

#include "IShowable.h"
#include "logic.h"
#include <bitset>
#include <functional>
#include <queue>
#include "ThreadPool.h"

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

	logic::weightedActionState getBestMove(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, int timeLeft);

	static int advanceHeuristic(std::bitset<128U>& state, bool isBlack);

	static int basicHeuristic(std::bitset<128U>&, bool);

private:
	const static int threadNumber = 4;
	ThreadPool threadPool;
	std::condition_variable cv;
	std::mutex blocker;
	std::mutex mtQ;
	std::mutex mtVal;
	bool returned = false;
	int counter = 0;
	sf::Clock clock;
	heuristic h;
	logic::weightedActionState alphaBeta(std::bitset<128U>& state, bool isBlack, unsigned int& moveLeft, int& timeLeft);
	std::pair<logic::weightedActionState, int> maxTop(std::bitset<128U>& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int & timeLeft, int alpha, int beta);
	int maxValue(std::bitset<128U>& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int alpha, int beta);
	int minValue(std::bitset<128U>& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int alpha, int beta);
	bool terminateTest(std::bitset<128U>& state, bool isBlack, unsigned int depth, unsigned int moveLeft);

};

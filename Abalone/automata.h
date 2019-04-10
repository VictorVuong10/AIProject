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
	typedef std::function<int(logic::bitState&, bool, int&, int&)> heuristic;

	typedef std::function<int(std::bitset<128U>&, bool)> heuristic_old;

	struct maxTopReturn {
		logic::weightedActionState bestAs;
		int bestV;
		bool completed;
	};

	struct maxTopReturn_old {
		logic::weightedActionState_old bestAs;
		int bestV;
		bool completed;
	};

	static const std::bitset<128U> MASKS_BLACK;
	static const std::bitset<128U> MASKS_WHITE;

	static constexpr int MIDDLE_H[61] = {
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

	automata(heuristic_old h);

	automata(heuristic h);

	~automata();

#pragma region old

	logic::weightedActionState_old getBestMove(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, int timeLeft);

	static int advanceHeuristic(std::bitset<128U>& state, bool isBlack);

	static int basicHeuristic(std::bitset<128U>&, bool);

#pragma endregion

	logic::weightedActionState getBestMove(logic::bitState& state, bool isBlack, unsigned int moveLeft, int timeLeft);

	static int h1(logic::bitState& state, bool isBlack, int& blackLost, int& whiteLost);

	static int h2(logic::bitState&, bool, int& blackLost, int& whiteLost);

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
	heuristic_old h;
	heuristic hn;

#pragma region old

	logic::weightedActionState_old alphaBeta(std::bitset<128U>& state, bool isBlack, unsigned int& moveLeft, int& timeLeft);
	maxTopReturn_old maxTop(std::multiset<logic::weightedActionState_old, std::greater<logic::weightedActionState_old>>& actionStates,
		std::bitset<128U>& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int & timeLeft, int alpha, int beta);
	int maxValue(std::bitset<128U>& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int alpha, int beta);
	int minValue(std::bitset<128U>& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int alpha, int beta);
	bool terminateTest(std::bitset<128U>& state, unsigned int depth, unsigned int moveLeft);

#pragma endregion

	logic::weightedActionState alphaBeta(logic::bitState& state, bool isBlack, unsigned int& moveLeft, int& timeLeft);
	maxTopReturn maxTop(std::multiset<logic::weightedActionState, std::greater<logic::weightedActionState>>& actionStates,
		logic::bitState& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int & timeLeft, int alpha, int beta);
	int maxValue(logic::bitState& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int alpha, int beta);
	int minValue(logic::bitState& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int alpha, int beta);
};

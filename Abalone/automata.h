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

	struct maxTopGlobal {
		std::bitset<128U> state;
		bool isBlack;
		int alpha;
		int beta;
		unsigned int depth;
		unsigned int moveLeft;
		int time;
		std::vector<std::pair<logic::action, std::bitset<128>>> actionStates;
		bool stopped;
		int bestV;
		std::pair<logic::action, std::bitset<128>> bestAs;
		maxTopGlobal() = default;
		maxTopGlobal(std::bitset<128U> state, bool isBlack, unsigned int depth, unsigned int moveLeft, int time,
			std::vector<std::pair<logic::action, std::bitset<128>>> actionStates)
			:state{ state }, isBlack{isBlack}, alpha{ INT_MIN }, beta{ INT_MAX }, depth{ depth },
			moveLeft{ moveLeft }, time{ time }, actionStates{ std::move(actionStates) }, stopped{ false }, bestV{ INT_MIN }, bestAs{}{};
	};

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

	automata(std::function<int(std::bitset<128U>&, bool)> & h);

	~automata();

	std::pair<logic::action, std::bitset<128>> getBestMove(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, int timeLeft);

private:
	const static int threadNumber = 4;
	ThreadPool threadPool;
	std::condition_variable cv;

	std::mutex blocker;
	std::mutex mtQ;
	std::mutex mtVal;
	maxTopGlobal sharedVal;

	int counter = 0;
	sf::Clock clock;
	heuristic h;
	std::pair<logic::action, std::bitset<128>> alphaBeta(std::bitset<128U>& state, bool isBlack, unsigned int& moveLeft, int& timeLeft);
	std::pair<std::pair<logic::action, std::bitset<128>>, int> maxTop(std::bitset<128U>& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int & timeLeft, int alpha, int beta);
	int maxValue(std::bitset<128U>& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int alpha, int beta);
	int minValue(std::bitset<128U>& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int alpha, int beta);
	bool terminateTest(std::bitset<128U>& state, bool isBlack, unsigned int depth, unsigned int moveLeft);

	int basicHeuristic(std::bitset<128U>&, bool);
};

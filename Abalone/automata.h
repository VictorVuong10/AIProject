#pragma once

#include "IShowable.h"
#include "logic.h"
#include <bitset>
#include <functional>
#include <queue>
#include <list>
#include <unordered_map>
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

	struct bitStateHash {
		std::size_t operator() (const logic::bitState & bs) const {
			return std::hash<unsigned long long>()(bs._1) ^ std::hash<unsigned long long>()(bs._2);
		}
	};

	struct tableEntry {
		//1: bestV, 2:alpha, 3:beta
		char flag;
		unsigned int depth;
		int val;
	};

	struct writeQueue {
		std::queue<std::function<void()>> q;
	};

	static const std::bitset<128U> MASKS_BLACK;
	static const std::bitset<128U> MASKS_WHITE;
	
	static constexpr unsigned long long BLACK_2 = 0b0000001010101010101010101010101010101010101010101010101010101010;
	static constexpr unsigned long long BLACK_1 = 0b1010101010101010101010101010101010101010101010101010101010101010;
	static constexpr unsigned long long WHITE_2 = 0b0000000101010101010101010101010101010101010101010101010101010101;
	static constexpr unsigned long long WHITE_1 = 0b0101010101010101010101010101010101010101010101010101010101010101;
	static constexpr unsigned long long SCORE_B = 0b1110000000000000000000000000000000000000000000000000000000000000;
	static constexpr unsigned long long SCORE_W = 0b0001110000000000000000000000000000000000000000000000000000000000;

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
	std::mutex write_blocker;
	std::mutex mtQ;
	std::mutex mtVal;

	std::unordered_map<logic::bitState, tableEntry, bitStateHash> * transTable;
	std::queue<writeQueue *> writeQueues;

	bool returned = false;
	int hitCounter = 0;
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
	int maxValue(writeQueue * curQueue, logic::bitState& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int alpha, int beta);
	int minValue(writeQueue * curQueue, logic::bitState& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int alpha, int beta);
	void writeAll();
	void addWriteTask(writeQueue * curQueue, logic::bitState & state, char flag, unsigned int & depth, int & val);
};

#pragma once

#include "logic.h"
#include <bitset>
#include <functional>

class automata
{
public:
	typedef std::function<int(std::bitset<128U>&, bool)> heuristic;

	static const std::string MASKS_STR[7];

	static const std::bitset<128U> MASKS_MID5;
	static const std::bitset<128U> MASKS_MID4;
	static const std::bitset<128U> MASKS_MID3;
	static const std::bitset<128U> MASKS_MID2;
	static const std::bitset<128U> MASKS_MID1;
	static const std::bitset<128U> MASKS_BLACK;
	static const std::bitset<128U> MASKS_WHITE;

	automata();

	automata(heuristic h);

	~automata();

	std::pair<logic::action, std::bitset<128>> getBestMove(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, unsigned int timeLeft);

private:
	int counter = 0;
	heuristic h;
	std::pair<logic::action, std::bitset<128>> alphaBeta(std::bitset<128U>& state, bool isBlack, unsigned int& moveLeft, unsigned int& timeLeft);
	std::pair<logic::action, std::bitset<128>> maxTop(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, unsigned int& timeLeft, int alpha, int beta);
	int maxValue(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, unsigned int& timeLeft, int alpha, int beta);
	int minValue(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, unsigned int& timeLeft, int alpha, int beta);
	bool terminateTest(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft);

	int basicHeuristic(std::bitset<128U>&, bool);
};


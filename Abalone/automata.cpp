#include "automata.h"



automata::automata() : automata{ std::bind(&automata::basicHeuristic, this, std::placeholders::_1, std::placeholders::_2) }
{
}

automata::automata(heuristic h) : h{ h }
{
}


automata::~automata()
{
}

std::pair<logic::action, std::bitset<128>> automata::getBestMove(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, unsigned int timeLeft)
{
	//clock.restart();
	return alphaBeta(state, isBlack, moveLeft, timeLeft);
}

std::pair<logic::action, std::bitset<128>> automata::alphaBeta(std::bitset<128U>& state, bool isBlack, unsigned int & moveLeft, unsigned int & timeLeft)
{
	return maxTop(state, isBlack, 4, timeLeft, INT_MIN, INT_MAX);
}

std::pair<logic::action, std::bitset<128>> automata::maxTop(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, unsigned int & timeLeft, int alpha, int beta)
{
	auto actionStates = logic::getAllValidMove(state, isBlack);
	int bestV = INT_MIN;
	std::pair<logic::action, std::bitset<128>> bestAs;
	//threading
	for (auto as : actionStates) {
		int curV = minValue(as.second, !isBlack, moveLeft - 1, timeLeft, alpha, beta);
		if (curV > bestV) {
			bestV = curV;
			bestAs = as;
		}
		alpha = std::max(alpha , bestV);
	}

	return bestAs;
}

int automata::maxValue(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, unsigned int & timeLeft, int alpha, int beta)
{
	if (terminateTest(state, isBlack, moveLeft)) {
		return h(state, isBlack);
	}
	auto actionStates = logic::getAllValidMove(state, isBlack);
	int bestV = INT_MIN;
	for (auto as : actionStates) {
		bestV = std::max(bestV, minValue(as.second, !isBlack, moveLeft - 1, timeLeft, alpha, beta));
		if (bestV > beta)
			return INT_MAX;
		alpha = std::max(alpha, bestV);
	}
	return bestV;
}

int automata::minValue(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, unsigned int & timeLeft, int alpha, int beta)
{
	if (terminateTest(state, isBlack, moveLeft)) {
		return h(state, isBlack);
	}
	auto actionStates = logic::getAllValidMove(state, isBlack);
	int bestV = INT_MAX;
	for (auto as : actionStates) {
		bestV = std::min(bestV, maxValue(as.second, !isBlack, moveLeft - 1, timeLeft, alpha, beta));
		if (bestV < alpha)
			return INT_MIN;
		beta = std::min(beta, bestV);
	}
	return bestV;
}

bool automata::terminateTest(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft)
{
	//time ran out
	//move ran out
	//score check
	if (moveLeft < 1) {
		return true;
	}
	auto scores = logic::getScoreFromState(state);
	return scores.x > 5 || scores.y > 5;
}

int automata::basicHeuristic(std::bitset<128U>&, bool)
{
	return rand();
}

#include "automata.h"

const std::string automata::MASKS_STR[7] = 
	{ 
		/* mid 1
			0000000000
		   000000000000
		  00000000000000
		 0000000000000000
		000000001100000000
		 0000000000000000
		  00000000000000
		   000000000000
			0000000000
		*/
		"0000000000000000000000000000000000000000000000000000000000000000000011000000000000000000000000000000000000000000000000000000000000",

		/* mid 2
			0000000000
		   000000000000
		  00000000000000
		 0000001111000000
		000000110011000000
		 0000001111000000
		  00000000000000
		   000000000000
			0000000000
		*/
		"0000000000000000000000000000000000000000000000000011110000000000001100110000000000001111000000000000000000000000000000000000000000",

		/* mid 3
			0000000000
		   000000000000
		  00001111110000
		 0000110000110000
		000011000000110000
		 0000110000110000
		  00001111110000
		   000000000000
			0000000000
		*/
		"00000000000000000000000000000000111111000000001100001100000000110000001100000000110000110000000011111100000000000000000000000000",

		/* mid 4
			0000000000
		   001111111100
		  00110000001100
		 0011000000001100
		001100000000001100
		 0011000000001100
		  00110000001100
		   001111111100
			0000000000
		*/
		"00000000000000000011111111000011000000110000110000000011000011000000000011000011000000001100001100000011000011111111000000000000",

		/* mid 5
			1111111111
		   110000000011
		  11000000000011
		 1100000000000011
		110000000000000011
		 1100000000000011
		  11000000000011
		   110000000011
			1111111111
		*/
		"00000011111111111100000000111100000000001111000000000000111100000000000000111100000000000011110000000000111100000000111111111111",
		
		/* black
			1010101010
		   101010101010
		  10101010101010
		 1010101010101010
		101010101010101010
		 1010101010101010
		  10101010101010
		   101010101010
			1010101010
		*/
		"00000010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010",

		/* white
			0101010101
		   010101010101
		  01010101010101
		 0101010101010101
		010101010101010101
		 0101010101010101
		  01010101010101
		   010101010101
			0101010101
		*/
		"00000001010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101"
	};


const std::bitset<128U> automata::MASKS_MID5{automata::MASKS_STR[0]};
const std::bitset<128U> automata::MASKS_MID4{automata::MASKS_STR[1]};
const std::bitset<128U> automata::MASKS_MID3{automata::MASKS_STR[2]};
const std::bitset<128U> automata::MASKS_MID2{automata::MASKS_STR[3]};
const std::bitset<128U> automata::MASKS_MID1{automata::MASKS_STR[4]};
const std::bitset<128U> automata::MASKS_BLACK{automata::MASKS_STR[5]};
const std::bitset<128U> automata::MASKS_WHITE{automata::MASKS_STR[6]};

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
	counter = 0;
	return alphaBeta(state, isBlack, moveLeft, timeLeft);
}

std::pair<logic::action, std::bitset<128>> automata::alphaBeta(std::bitset<128U>& state, bool isBlack, unsigned int & moveLeft, unsigned int & timeLeft)
{
	auto asd = maxTop(state, isBlack, 3, timeLeft, INT_MIN, INT_MAX);
	std::cout << counter << std::endl;
	return asd;
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
		return h(state, !isBlack);
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
	++counter;
	//time ran out
	//move ran out
	//score check
	if (moveLeft < 1) {
		return true;
	}
	auto scores = logic::getScoreFromState(state);
	return scores.x > 5 || scores.y > 5;
}

int automata::basicHeuristic(std::bitset<128U>& state, bool isBlack)
{
	auto scores = logic::getScoreFromState(state);
	auto extracted = isBlack ? state & MASKS_BLACK : state & MASKS_WHITE;
	auto mid5 = extracted & MASKS_MID5;
	auto mid4 = extracted & MASKS_MID4;
	auto mid3 = extracted & MASKS_MID3;
	auto mid2 = extracted & MASKS_MID2;
	auto mid1 = extracted & MASKS_MID1;
	auto res = 0u;
	for (auto i = isBlack << 0; i < 122; i += 2) {
		res += mid5[i] * 5;
		res += mid4[i] * 4;
		res += mid3[i] * 3;
		res += mid2[i] * 2;
		res += mid1[i] * 1;
	}
	return res;
	//if (isBlack) {
	//	return scores.y - scores.x;
	//}
	//else {
	//	return scores.x - scores.y;
	//}
}

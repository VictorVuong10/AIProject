#include "automata.h"
#include "game.h"

const std::bitset<128U> automata::MASKS_BLACK{ "00000010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010" };
const std::bitset<128U> automata::MASKS_WHITE{ "00000001010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101" };

automata::automata() : automata{ std::bind(&automata::basicHeuristic, this, std::placeholders::_1, std::placeholders::_2) }
{
}

automata::automata(int test) : automata{ std::bind(&automata::johnHeuristic, this, std::placeholders::_1, std::placeholders::_2) }
{

}

automata::automata(heuristic h) : h{ h }
{
}


automata::~automata()
{
}

std::pair<logic::action, std::bitset<128>> automata::getBestMove(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, int timeLeft)
{
	clock.restart();
	counter = 0;
	if (moveLeft < 1) moveLeft = 4;
	return alphaBeta(state, isBlack, moveLeft, timeLeft);
}

std::pair<logic::action, std::bitset<128>> automata::alphaBeta(std::bitset<128U>& state, bool isBlack, unsigned int & moveLeft, int& timeLeft)
{	
	timeLeft *= 1000;
	auto depth = 1u;
	int miliSec = clock.getElapsedTime().asMilliseconds();
	int lastLayerUsed = 0;
	std::pair<logic::action, std::bitset<128>> best;
	do {
		best = maxTop(state, isBlack, depth, moveLeft, timeLeft, INT_MIN, INT_MAX);
		lastLayerUsed = clock.getElapsedTime().asMilliseconds() - miliSec;
		miliSec += lastLayerUsed;

		std::cout << "Depth: " << depth << std::endl;
		std::cout << "Time used: " << lastLayerUsed << std::endl;
		std::cout << "Searched state: " << counter << std::endl;
		std::cout << "Best Action: " << best.first.count << " "<< best.first.direction << " " << best.first.index << std::endl;

		++depth;
	} while (timeLeft- miliSec > lastLayerUsed);
	return best;
}

std::pair<logic::action, std::bitset<128>> automata::maxTop(std::bitset<128U>& state, bool isBlack, unsigned int depth,
	unsigned int moveLeft, int& timeLeft, int alpha, int beta)
{
	auto actionStates = logic::getAllValidMove(state, isBlack);
	int bestV = INT_MIN;
	std::pair<logic::action, std::bitset<128>> bestAs;
	//threading
	for (auto as : actionStates) {
		int curV = minValue(as.second, !isBlack, depth - 1, moveLeft - 1, timeLeft, alpha, beta);
		if (curV > bestV) {
			bestV = curV;
			bestAs = as;
		}
		alpha = std::max(alpha , bestV);
	}

	return bestAs;
}

int automata::maxValue(std::bitset<128U>& state, bool isBlack, unsigned int depth,
	unsigned int moveLeft, int& timeLeft, int alpha, int beta)
{
	if (terminateTest(state, isBlack, depth, moveLeft, timeLeft)) {
		return h(state, isBlack);
	}
	auto actionStates = logic::getAllValidMove(state, isBlack);
	int bestV = INT_MIN;
	for (auto as : actionStates) {
		bestV = std::max(bestV, minValue(as.second, !isBlack, depth - 1, moveLeft - 1, timeLeft, alpha, beta));
		if (bestV > beta)
			return INT_MAX;
		alpha = std::max(alpha, bestV);
	}
	return bestV;
}

int automata::minValue(std::bitset<128U>& state, bool isBlack, unsigned int depth,
	unsigned int moveLeft, int& timeLeft, int alpha, int beta)
{
	if (terminateTest(state, !isBlack, depth, moveLeft, timeLeft)) {
		return h(state, !isBlack);
	}
	auto actionStates = logic::getAllValidMove(state, isBlack);
	int bestV = INT_MAX;
	for (auto as : actionStates) {
		bestV = std::min(bestV, maxValue(as.second, !isBlack, depth - 1, moveLeft - 1, timeLeft, alpha, beta));
		if (bestV < alpha)
			return INT_MIN;
		beta = std::min(beta, bestV);
	}
	return bestV;
}

bool automata::terminateTest(std::bitset<128U>& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int& timeLeft)
{
	++counter;
	//depth ran out
	//move ran out
	//time ran out
	//score check
	if (depth < 1 || moveLeft < 1 || timeLeft - clock.getElapsedTime().asMilliseconds() < 50) {
		return true;
	}
	auto scores = logic::getScoreFromState(state);
	return scores.x > 5 || scores.y > 5;
}

//int automata::basicHeuristic(std::bitset<128U>& state, bool isBlack)
//{
//	auto scores = logic::getScoreFromState(state);
//	auto scoreMean = isBlack ? scores.y - scores.x : scores.x - scores.y;
//	auto extracted = isBlack ? state & MASKS_BLACK : state & MASKS_WHITE;
//	auto midMean = 0u;
//	for (auto i = isBlack << 0; i < 122; i += 2) {
//		if (state[i]) {
//			midMean += MIDDLE_H[i >> 1];
//		}
//	}
//	return midMean + 1000 * scoreMean;
//}

int automata::basicHeuristic(std::bitset<128U>& state, bool isBlack)
{
	auto scores = logic::getScoreFromState(state);
	auto scoreDifference = isBlack ? scores.y - scores.x : scores.x - scores.y;
	int adjacencyScoreWhite = 0;
	int adjacencyScoreBlack = 0;
	int adjacencyThis = 0;
	int adjacencyOpp = 0;
	bool left;
	bool topLeft;
	bool topRight;
	bool right;
	bool downRight;
	bool downLeft;
	//Not yet
	//auto extracted = isBlack ? state & MASKS_BLACK : state & MASKS_WHITE;
	auto midMean = 0u;
	auto midMeanOpp = 0u;
	for (auto i = isBlack << 0, k = !isBlack << 0; i < 122; i += 2, k += 2) {
		//std::cout << "I " << i << std::endl;
		if (state[i]) {
			midMean += MIDDLE_H_VIC[i >> 1];
		}
		else if (state[k]) {
				midMeanOpp += MIDDLE_H_OPP[k >> 1];
		}
		//Adjacency score maybe use bitwise with mask instead?
		//Maybe use average distance between all marbles?
		if (state[i] == 1)
		{
			//std::cout << "white" << std::endl;
			int position = i >> 1;
			//std::cout << "position" << position << std::endl;
			//std::cout << "state" << (logic::MOVE_TABLE[position][1] << 1) << std::endl;
			left = logic::MOVE_TABLE[position][0] << 1 >= 0 && logic::MOVE_TABLE[position][0] << 1 <= 122
				&& state[logic::MOVE_TABLE[position][0] << 1] == 1 && state[(logic::MOVE_TABLE[position][0] << 1) + 1] == 0;
			topLeft = logic::MOVE_TABLE[position][1] << 1 >= 0 && logic::MOVE_TABLE[position][1] << 1 <= 122
				&& state[logic::MOVE_TABLE[position][1] << 1] == 1 && state[(logic::MOVE_TABLE[position][1] << 1) + 1] == 0;
			topRight = logic::MOVE_TABLE[position][2] << 1 >= 0 && logic::MOVE_TABLE[position][2] << 1 <= 122
				&& state[logic::MOVE_TABLE[position][2] << 1] == 1 && state[(logic::MOVE_TABLE[position][2] << 1) + 1] == 0;


			right = (logic::MOVE_TABLE[position][3] << 1) >= 0 && (logic::MOVE_TABLE[position][3] << 1) <= 122
				&& state[logic::MOVE_TABLE[position][3] << 1] == 1 && state[(logic::MOVE_TABLE[position][3] << 1) + 1] == 0;
			downRight = logic::MOVE_TABLE[position][4] << 1 >= 0 && logic::MOVE_TABLE[position][4] << 1 <= 122
				&& state[logic::MOVE_TABLE[position][4] << 1] == 1 && state[(logic::MOVE_TABLE[position][4] << 1) + 1] == 0;
			downLeft = logic::MOVE_TABLE[position][5] << 1 >= 0 && logic::MOVE_TABLE[position][5] << 1 <= 122
				&& state[logic::MOVE_TABLE[position][5] << 1] == 1 && state[(logic::MOVE_TABLE[position][5] << 1) + 1] == 0;
			//std::cout << "before adj" << std::endl;
			int adj = 1;
			//highly clustered = exponential score
			adj << left << topLeft << topRight << right << downRight << downLeft;
			adjacencyScoreWhite += adj;
		}
		else if (state[i + 1] == 1)
		{
			//std::cout << "black" << std::endl;
			int position = i >> 1;
			//std::cout << "position" << position << std::endl;
			//std::cout << "state" << (logic::MOVE_TABLE[position][1] << 1) << std::endl;
			left = logic::MOVE_TABLE[position][0] << 1 >= 0 && logic::MOVE_TABLE[position][0] << 1 <= 122
				&& state[logic::MOVE_TABLE[position][0] << 1] == 0 && state[(logic::MOVE_TABLE[position][0] << 1) + 1] == 1;
			topLeft = logic::MOVE_TABLE[position][1] << 1 >= 0 && logic::MOVE_TABLE[position][1] << 1 <= 122
				&& state[logic::MOVE_TABLE[position][1] << 1] == 0 && state[(logic::MOVE_TABLE[position][1] << 1) + 1] == 1;
			topRight = logic::MOVE_TABLE[position][2] << 1 >= 0 && logic::MOVE_TABLE[position][2] << 1 <= 122
				&& state[logic::MOVE_TABLE[position][2] << 1] == 0 && state[(logic::MOVE_TABLE[position][2] << 1) + 1] == 1;


			right = (logic::MOVE_TABLE[position][3] << 1) >= 0 && (logic::MOVE_TABLE[position][3] << 1) <= 122
				&& state[logic::MOVE_TABLE[position][3] << 1] == 0 && state[(logic::MOVE_TABLE[position][3] << 1) + 1] == 1;
			downRight = logic::MOVE_TABLE[position][4] << 1 >= 0 && logic::MOVE_TABLE[position][4] << 1 <= 122
				&& state[logic::MOVE_TABLE[position][4] << 1] == 0 && state[(logic::MOVE_TABLE[position][4] << 1) + 1] == 1;
			downLeft = logic::MOVE_TABLE[position][5] << 1 >= 0 && logic::MOVE_TABLE[position][5] << 1 <= 122
				&& state[logic::MOVE_TABLE[position][5] << 1] == 0 && state[(logic::MOVE_TABLE[position][5] << 1) + 1] == 1;
			//std::cout << "before adj" << std::endl;
			int adj = 1;
			//highly clustered = exponential score
			adj << left << topLeft << topRight << right << downRight << downLeft;
			adjacencyScoreBlack += adj;
		}
	}

	if (isBlack) {
		adjacencyOpp = (1 / adjacencyScoreWhite) * 7;
		adjacencyThis = adjacencyScoreBlack;
	}
	else {
		adjacencyOpp = (1 / adjacencyScoreBlack) * 7;
		adjacencyThis = adjacencyScoreWhite;
	}

	//i need a check that does if the score is >2 then make it 0 again want to be on defense perm

	//this wont work we need a way to keep the movescore in the state?
	//int moveScore;
	//if (game::movesMade == 0) {
	//	 moveScore = 1;
	//}
	//else {
	//	 moveScore = game::movesMade;
	//}

	//if (midMean < 30) {
	//	return (((6*midMean) + (adjacencyThis*8) + (adjacencyOpp*2) + (midMeanOpp * 2) + (20 * scoreDifference)) / moveScore);
	//}

	//if (midMeanOpp < 20) {

	//	return (((20 * midMean * adjacencyThis) + (midMeanOpp * 2) + (2 * adjacencyOpp) + (10 * scoreDifference)) / moveScore);
	//}

	if (midMean > 31 && midMeanOpp <30) {
		return (((33 * midMean * adjacencyThis) + (midMeanOpp * 12 * adjacencyOpp) + (2000 * scoreDifference)));
	}

	if (midMean > 31 && midMeanOpp >= 30) {
		return (((35 * midMean * adjacencyThis) + (midMeanOpp * 25 * adjacencyOpp) + (3500 * scoreDifference)));
	}

	if (midMeanOpp > 24) {

		return (((35 * midMean * adjacencyThis) + (midMeanOpp * 30 * adjacencyOpp) + (5500 * scoreDifference)));
	}

}


int automata::johnHeuristic(std::bitset<128U>& state, bool isBlack)
{
	auto scores = logic::getScoreFromState(state);
	auto scoreDifference = isBlack ? scores.y - scores.x : scores.x - scores.y;
	int adjacencyScore = 0;
	bool left;
	bool topLeft;
	bool topRight;
	bool right;
	bool downRight;
	bool downLeft;
	//Not yet
	//auto extracted = isBlack ? state & MASKS_BLACK : state & MASKS_WHITE;
	auto midMean = 0u;
	auto midMeanOpp = 0u;
	for (auto i = isBlack << 0, k = !isBlack << 0; i < 122; i += 2, k+=2) {
		//std::cout << "I " << i << std::endl;
		if (state[i]) {
			midMean += MIDDLE_H[i >> 1];
		}
		else if (state[k]) {

			if (k == 31) {
				midMeanOpp += 5;
			}
			else {

				midMeanOpp += MIDDLE_H[i >> 1];
			}


		}
		//Adjacency score maybe use bitwise with mask instead?
		//Maybe use average distance between all marbles?
		if (!isBlack && state[i] == 1 && state[i + 1] == 0)
		{
			//std::cout << "white" << std::endl;
			int position = i >> 1;
			//std::cout << "position" << position << std::endl;
			//std::cout << "state" << (logic::MOVE_TABLE[position][1] << 1) << std::endl;
			left = logic::MOVE_TABLE[position][0] << 1 >= 0 && logic::MOVE_TABLE[position][0] << 1 <=122
				&& state[logic::MOVE_TABLE[position][0] << 1] == 1 && state[(logic::MOVE_TABLE[position][0] << 1) + 1] == 0;
			topLeft = logic::MOVE_TABLE[position][1] << 1 >= 0 && logic::MOVE_TABLE[position][1] << 1 <= 122 
				&& state[logic::MOVE_TABLE[position][1] << 1] == 1 && state[(logic::MOVE_TABLE[position][1] << 1) + 1] == 0;
			topRight = logic::MOVE_TABLE[position][2] << 1 >= 0 && logic::MOVE_TABLE[position][2] << 1 <= 122 
				&& state[logic::MOVE_TABLE[position][2] << 1] == 1 && state[(logic::MOVE_TABLE[position][2]  << 1) + 1] == 0;


			right = (logic::MOVE_TABLE[position][3] << 1) >= 0 && (logic::MOVE_TABLE[position][3] << 1) <= 122 
				&& state[logic::MOVE_TABLE[position][3] << 1] == 1 && state[(logic::MOVE_TABLE[position][3] << 1) + 1] == 0;
			downRight = logic::MOVE_TABLE[position][4] << 1 >= 0 && logic::MOVE_TABLE[position][4] << 1 <= 122 
				&& state[logic::MOVE_TABLE[position][4] << 1] == 1 && state[(logic::MOVE_TABLE[position][4] << 1) + 1] == 0;
			downLeft = logic::MOVE_TABLE[position][5] << 1 >= 0 && logic::MOVE_TABLE[position][5] << 1 <= 122 
				&& state[logic::MOVE_TABLE[position][5] << 1] == 1 && state[(logic::MOVE_TABLE[position][5] << 1) + 1] == 0;
			//std::cout << "before adj" << std::endl;
			int adj = 1;
			//highly clustered = exponential score
			adj << left << topLeft << topRight << right << downRight << downLeft;
			adjacencyScore += adj;
		}
		else if (isBlack && state[i] == 0 && state[i + 1] == 1)
		{
			int position = i >> 1;
			//std::cout << "position" << position << std::endl;
			//std::cout << "state" << (logic::MOVE_TABLE[position][1] << 1) << std::endl;
			left = logic::MOVE_TABLE[position][0] << 1 >= 0 && logic::MOVE_TABLE[position][0] << 1 <= 122
				&& state[logic::MOVE_TABLE[position][0] << 1] == 0 && state[(logic::MOVE_TABLE[position][0] << 1) + 1] == 1;
			topLeft = logic::MOVE_TABLE[position][1] << 1 >= 0 && logic::MOVE_TABLE[position][1] << 1 <= 122
				&& state[logic::MOVE_TABLE[position][1] << 1] == 0 && state[(logic::MOVE_TABLE[position][1] << 1) + 1] == 1;
			topRight = logic::MOVE_TABLE[position][2] << 1 >= 0 && logic::MOVE_TABLE[position][2] << 1 <= 122
				&& state[logic::MOVE_TABLE[position][2] << 1] == 0 && state[(logic::MOVE_TABLE[position][2] << 1) + 1] == 1;


			right = (logic::MOVE_TABLE[position][3] << 1) >= 0 && (logic::MOVE_TABLE[position][3] << 1) <= 122
				&& state[logic::MOVE_TABLE[position][3] << 1] == 0 && state[(logic::MOVE_TABLE[position][3] << 1) + 1] == 1;
			downRight = logic::MOVE_TABLE[position][4] << 1 >= 0 && logic::MOVE_TABLE[position][4] << 1 <= 122
				&& state[logic::MOVE_TABLE[position][4] << 1] == 0 && state[(logic::MOVE_TABLE[position][4] << 1) + 1] == 1;
			downLeft = logic::MOVE_TABLE[position][5] << 1 >= 0 && logic::MOVE_TABLE[position][5] << 1 <= 122
				&& state[logic::MOVE_TABLE[position][5] << 1] == 0 && state[(logic::MOVE_TABLE[position][5] << 1) + 1] == 1;
			//std::cout << "before adj" << std::endl;
			int adj = 1;
			//highly clustered = exponential score
			adj << left << topLeft << topRight << right << downRight << downLeft;
			adjacencyScore += adj;
		}
	}

	return (50 * midMean * adjacencyScore) + ((1/midMeanOpp) * 8 * 50) + (5000 * scoreDifference);
}

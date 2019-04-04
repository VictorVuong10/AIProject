#include "automata.h"

const std::bitset<128U> automata::MASKS_BLACK{ "00000010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010" };
const std::bitset<128U> automata::MASKS_WHITE{ "00000001010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101" };

//ThreadPool automata::threadPool{ 4 };

automata::automata() : threadPool{threadNumber}
{
	h = std::bind(&automata::basicHeuristic, this, std::placeholders::_1, std::placeholders::_2);
}


automata::automata(heuristic h) : h{ h }, threadPool{ threadNumber }
{
}


automata::~automata()
{
}

std::pair<logic::action, std::bitset<128>> automata::getBestMove(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, int timeLeft)
{
	threadPool.wait();
	clock.restart();
	counter = 0;
	if (moveLeft == 0) moveLeft = 4;
	if (timeLeft == 0 || timeLeft > 100) timeLeft = 5;
	return alphaBeta(state, isBlack, moveLeft, timeLeft);
}

std::pair<logic::action, std::bitset<128>> automata::alphaBeta(std::bitset<128U>& state, bool isBlack, unsigned int & moveLeft, int& timeLeft)
{	
	timeLeft *= 1000;
	auto depth = 1u;
	int miliSec = clock.getElapsedTime().asMilliseconds();
	int lastLayerUsed = 0;
	std::pair<logic::action, std::bitset<128>> best;
	int bestV = INT_MIN;
	do {
		returned = false;
		auto r = maxTop(state, isBlack, depth, moveLeft, timeLeft, INT_MIN, INT_MAX);
		if (r.second > bestV) {
			best = r.first;
			bestV = r.second;
		}
		lastLayerUsed = clock.getElapsedTime().asMilliseconds() - miliSec;
		miliSec += lastLayerUsed;

		std::cout << "Depth: " << depth << std::endl;
		std::cout << "Time used: " << lastLayerUsed << std::endl;
		std::cout << "Searched state: " << counter << std::endl;
		std::cout << "Best Action: " << best.first.count << " "<< best.first.direction << " " << best.first.index << std::endl;

		++depth;
	} while (depth < moveLeft && timeLeft- miliSec > lastLayerUsed);
	return best;
}

std::pair<std::pair<logic::action, std::bitset<128>>, int> automata::maxTop(std::bitset<128U>& state, bool isBlack, unsigned int depth,
	unsigned int moveLeft, int& timeLeft, int alpha, int beta)
{
	auto actionStates = logic::getAllValidMoveOrdered(state, isBlack);
	int bestV = INT_MIN;
	std::pair<logic::action, std::bitset<128>> bestAs;

	/*unsigned short threadCount = 4;
	std::vector<std::thread> threadPool;
	size_t stateIndex = 0;

	for (auto i = 0; i < threadCount; ++i) {
		threadPool.emplace_back([this, &stateIndex, &alpha, &beta, &actionStates, &bestV, &bestAs, isBlack, depth, moveLeft]() {
			while (!returned) {
				std::pair < logic::action, std::bitset<128>> onePair;
				{
					if (stateIndex >= actionStates.size()) {
						cv.notify_all();
						returned = true;
						std::cout << "depth complete" << std::endl;
						return;
					}
					else {
						std::unique_lock<std::mutex>lock{ mtQ };
						onePair = actionStates[stateIndex++];
					}
				}
				int curV = minValue(onePair.second, !isBlack, depth - 1, moveLeft - 1, alpha, beta);
				if (returned)
					return;
				{
					std::unique_lock<std::mutex> lock{ mtVal };
					if (curV > bestV) {
						bestV = curV;
						bestAs = onePair;
					}
					alpha = std::max(alpha, bestV);
				}
			}
		});
	}
	std::unique_lock<std::mutex> blockLck{ blocker };
	cv.wait_for(blockLck, std::chrono::milliseconds{ timeLeft - clock.getElapsedTime().asMilliseconds() });
	std::cout << "returned" << std::endl;
	for (auto& t : threadPool) {
		t.detach();
	}
	return { bestAs, bestV };*/

	for (auto i = 0u; i < actionStates.size(); ++i) {
		auto actualTime = timeLeft - clock.getElapsedTime().asMilliseconds();
		auto curState = actionStates[i];
		threadPool.schedule([this, curState, &bestV, i, isBlack, depth, moveLeft, &alpha, beta, &bestAs]() {
			if (returned)
				return;
			auto oneState = curState.second;
			int curV = minValue(oneState, !isBlack, depth - 1, moveLeft - 1, alpha, beta);
			if (returned)
				return;
			std::unique_lock<std::mutex> lck{ mtQ };
			if (curV > bestV) {
				bestV = curV;
				bestAs = curState;
			}
			alpha = std::max(alpha, bestV);
			
			cv.notify_all();
		});
		if (i > threadNumber - 1) {
			std::unique_lock<std::mutex> blockLck{ blocker };
			if (cv.wait_for(blockLck, std::chrono::milliseconds{ actualTime }) == std::cv_status::timeout) {
				std::cout << "timeout" << std::endl;
				goto returning;
			}
		}
	}
	
	

	//threading
	/*for (auto as : actionStates) {
		int curV = minValue(as.second, !isBlack, depth - 1, moveLeft - 1, timeLeft, alpha, beta);
		if (curV > bestV) {
			bestV = curV;
			bestAs = as;
		}
		alpha = std::max(alpha , bestV);
	}*/
returning:
	std::cout << "returned" << std::endl;
	returned = true;
	return { bestAs, bestV };
}

int automata::maxValue(std::bitset<128U>& state, bool isBlack, unsigned int depth,
	unsigned int moveLeft, int alpha, int beta)
{
	if (returned)
		return INT_MAX;
	if (terminateTest(state, isBlack, depth, moveLeft)) {
		return h(state, isBlack);
	}
	auto actionStates = logic::getAllValidMoveOrdered(state, isBlack);
	int bestV = INT_MIN;
	for (auto as : actionStates) {
		bestV = std::max(bestV, minValue(as.second, !isBlack, depth - 1, moveLeft - 1, alpha, beta));
		if (bestV > beta)
			return INT_MAX;
		alpha = std::max(alpha, bestV);
	}
	return bestV;
}

int automata::minValue(std::bitset<128U>& state, bool isBlack, unsigned int depth,
	unsigned int moveLeft, int alpha, int beta)
{
	if (returned)
		return INT_MIN;
	if (terminateTest(state, !isBlack, depth, moveLeft)) {
		return h(state, !isBlack);
	}
	auto actionStates = logic::getAllValidMoveOrdered(state, isBlack);
	int bestV = INT_MAX;
	for (auto as : actionStates) {
		bestV = std::min(bestV, maxValue(as.second, !isBlack, depth - 1, moveLeft - 1, alpha, beta));
		if (bestV < alpha)
			return INT_MIN;
		beta = std::min(beta, bestV);
	}
	return bestV;
}

bool automata::terminateTest(std::bitset<128U>& state, bool isBlack, unsigned int depth, unsigned int moveLeft)
{
	++counter;
	//depth ran out
	//move ran out
	//time ran out
	//score check
	if (depth < 1 || moveLeft < 1) {
		return true;
	}
	auto scores = logic::getScoreFromState(state);
	return scores.x > 5 || scores.y > 5;
}

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
			midMean += MIDDLE_H[i >> 1];
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

	return (5 * midMean) + (adjacencyThis * 1.5) + (4 * midMeanOpp) + (adjacencyOpp * 1) + (200 * scoreDifference);
	//35, 30, 5500
	//50, 30, 5500 lost
	//50, 30, 5500 flipped sides, 
	//best so far return (5 * midMean) + (adjacencyThis * 2) + (4 * midMeanOpp) + (adjacencyOpp * 1) + (200 * scoreDifference);


}

int automata::testHeuristic(std::bitset<128U>& state, bool isBlack)
{
	auto scores = logic::getScoreFromState(state);
	auto scoreMean = isBlack ? scores.y - scores.x : scores.x - scores.y;
	/*auto extracted = isBlack ? state & MASKS_BLACK : state & MASKS_WHITE;*/
	auto midMean = 0u;
	for (auto i = isBlack << 0, j = isBlack ^ 1; i < 122; i += 2, j += 2) {
		if (state[i]) {
			midMean += MIDDLE_H[i >> 1];

		}
		if (state[j]) {
			midMean -= MIDDLE_H[j >> 1];
		}
	}
	return 5 * midMean + 200 * scoreMean;
}



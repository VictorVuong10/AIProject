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
	auto actionStates = logic::getAllValidMove(state, isBlack);
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
	auto actionStates = logic::getAllValidMove(state, isBlack);
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
	auto actionStates = logic::getAllValidMove(state, isBlack);
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
	auto scoreMean = isBlack ? scores.y - scores.x : scores.x - scores.y;
	/*auto extracted = isBlack ? state & MASKS_BLACK : state & MASKS_WHITE;*/
	auto midMean = 0u;
	auto ajacentMean = 0u;
	for (auto i = isBlack << 0, j = isBlack ^ 1; i < 122; i += 2, j += 2) {
		if (state[i]) {
			midMean += MIDDLE_H[i >> 1];
			for (auto k = 3u; k < 6; ++k) {
				auto adj = logic::MOVE_TABLE[i >> 1][k];
				if (adj != -1 && state[(adj << 1) + (isBlack << 0)]) {
					++ajacentMean;
				}
			}
		}
		if (state[j]) {
			midMean -= MIDDLE_H[j >> 1];
		}
	}
	return 5 * midMean + ajacentMean + 200 * scoreMean;
}

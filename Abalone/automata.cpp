#include "automata.h"

const std::bitset<128U> automata::MASKS_BLACK{ "00000010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010" };
const std::bitset<128U> automata::MASKS_WHITE{ "00000001010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101" };

automata::automata() : threadPool{ threadNumber }, h{ automata::basicHeuristic }, hn{automata::h1}{}

automata::automata(heuristic_old h) : h{ h }, hn{ automata::h1 }, threadPool{ threadNumber }{}

automata::automata(heuristic h) : h{ automata::basicHeuristic }, hn{ h }, threadPool{ threadNumber }{}

automata::~automata()
{
}

logic::weightedActionState_old automata::getBestMove(std::bitset<128U>& state, bool isBlack, unsigned int moveLeft, int timeLeft)
{
	std::cout << state.to_string() << std::endl;
	threadPool.wait();
	clock.restart();
	counter = 0;
	if (moveLeft == 0) moveLeft = 4;
	if (timeLeft == 0 || timeLeft > 100) timeLeft = 5;
	timeLeft *= 1000;
	return alphaBeta(state, isBlack, moveLeft, timeLeft);
}

logic::weightedActionState_old automata::alphaBeta(std::bitset<128U>& state, bool isBlack, unsigned int & moveLeft, int& timeLeft)
{	
	auto depth = 3u;
	int miliSec = clock.getElapsedTime().asMilliseconds();
	int lastLayerUsed = 0;
	logic::weightedActionState_old best;
	int bestV = INT_MIN;
	auto actionStates = logic::getAllValidMoveOrdered(state, isBlack);
	do {
		returned = false;
		auto r = maxTop(actionStates, state, isBlack, depth, moveLeft, timeLeft, INT_MIN, INT_MAX);
		if (r.completed) {
			best = r.bestAs;
			bestV = r.bestV;
		}
		lastLayerUsed = clock.getElapsedTime().asMilliseconds() - miliSec;
		miliSec += lastLayerUsed;

		std::cout << "Depth: " << depth << std::endl;
		std::cout << "Time used: " << lastLayerUsed << std::endl;
		std::cout << "Searched state: " << counter << std::endl;
		std::cout << "bestV: " << bestV << std::endl;
		std::cout << "Best Action: " << best.act.act.count << " "<< best.act.act.direction << " " << best.act.act.index << std::endl;

		++depth;
	} while (depth < moveLeft && timeLeft- miliSec > (lastLayerUsed << 3));
	return best;
}

automata::maxTopReturn_old automata::maxTop(std::multiset<logic::weightedActionState_old, std::greater<logic::weightedActionState_old>>& actionStates,
	std::bitset<128U>& state, bool isBlack, unsigned int depth, unsigned int moveLeft, int& timeLeft, int alpha, int beta)
{
	int bestV = INT_MIN;
	logic::weightedActionState_old bestAs;
	auto iter = actionStates.begin();
	int threadRemain = threadNumber;
	int completedBranch = 0;
	for (auto i = 0u; iter != actionStates.end(); ++i, ++iter) {
		//take a thread
		--threadRemain;
		threadPool.schedule([this, iter, &bestV, &bestAs, &completedBranch, &threadRemain ,isBlack, depth, moveLeft, &alpha, beta]() {
			std::bitset<128U> oneState;
			{
				std::unique_lock<std::mutex> lck{ mtQ };
				if (returned) {
					return;
				}
				oneState = iter->state;
			}
			int curV = minValue(oneState, !isBlack, depth - 1, moveLeft - 1, alpha, beta);
			std::unique_lock<std::mutex> lck{ mtQ };
			if (returned)
				return;
			//// && curState.act.act.count == 1 && curState.act.act.direction == 5 && curState.act.act.index == 55
			//if (depth == 4) {
			//	std::cout << "innerC: " << completedBranch << std::endl;
			//	std::cout << "+++++++++++count " << curState.act.act.count << " index: "<< curState.act.act.index << " direction: " << curState.act.act.direction << std::endl;
			//	std::cout << "curV: " << curV << std::endl << std::endl;
			//}
			if (curV > bestV) {
				bestV = curV;
				bestAs = *iter;
			}
			alpha = std::max(alpha, bestV);
			
			
			//return a thread
			++threadRemain;
			//increase completed branch
			++completedBranch;
			//notify cv thread made change.
			cv.notify_one();
		});
		//wait for there is remaining thread, if timeout then set cutoff variable then return.
		std::unique_lock<std::mutex> blockLck{ blocker };
		if (!cv.wait_for(blockLck, std::chrono::milliseconds{ timeLeft - clock.getElapsedTime().asMilliseconds() }, [&threadRemain] {return threadRemain > 0; })) {
			std::unique_lock<std::mutex> lck{ mtQ };
			std::cout << "<timeout>" << std::endl;
			returned = true;
			return { bestAs, bestV, false };
		}
	}
	//wait for all branch are completed, if timeout then set cutoff variable then return.
	std::unique_lock<std::mutex> blockLck{ blocker };
	if (!cv.wait_for(blockLck, std::chrono::milliseconds{ timeLeft - clock.getElapsedTime().asMilliseconds() },
		[&completedBranch, &actionStates] {return completedBranch == actionStates.size(); })) {
		std::unique_lock<std::mutex> lck{ mtQ };
		std::cout << "<timeout>" << std::endl;
		returned = true;
		return { bestAs, bestV, false };
	}
	
	
	std::unique_lock<std::mutex> lck{ mtQ };
	std::cout << "<completed>" << std::endl;
	//std::cout << "----------------inner Counter:" << completedBranch << " actionLen: "<< actionStates.size() << std::endl;
	returned = true;

	//single thread
	/*for (auto as : actionStates) {
		int curV = minValue(as.state, !isBlack, depth - 1, moveLeft - 1, alpha, beta);
		if (curV > bestV) {
			bestV = curV;
			bestAs = as;
		}
		alpha = std::max(alpha , bestV);
	}*/
	return { bestAs, bestV, true };
}

int automata::maxValue(std::bitset<128U>& state, bool isBlack, unsigned int depth,
	unsigned int moveLeft, int alpha, int beta)
{
	if (returned)
		return INT_MAX;
	if (terminateTest(state, depth, moveLeft)) {
		return h(state, isBlack);
	}
	auto actionStates = logic::getAllValidMoveOrdered(state, isBlack);
	int bestV = INT_MIN;
	for (auto as : actionStates) {
		bestV = std::max(bestV, minValue(as.state, !isBlack, depth - 1, moveLeft - 1, alpha, beta));
		if (bestV >= beta)
			return bestV;
		alpha = std::max(alpha, bestV);
	}
	return bestV;
}

int automata::minValue(std::bitset<128U>& state, bool isBlack, unsigned int depth,
	unsigned int moveLeft, int alpha, int beta)
{
	if (returned)
		return INT_MIN;
	if (terminateTest(state, depth, moveLeft)) {
		return h(state, !isBlack);
	}
	auto actionStates = logic::getAllValidMoveOrdered(state, isBlack);
	int bestV = INT_MAX;
	for (auto as : actionStates) {
		bestV = std::min(bestV, maxValue(as.state, !isBlack, depth - 1, moveLeft - 1, alpha, beta));
		if (bestV <= alpha)
			return bestV;
		beta = std::min(beta, bestV);
	}
	
	return bestV;
}

bool automata::terminateTest(std::bitset<128U>& state, unsigned int depth, unsigned int moveLeft)
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

int automata::advanceHeuristic(std::bitset<128U>& state, bool isBlack)
{
	auto scores = logic::getScoreFromState(state);
	int scoreMean = 0;
	if (isBlack) {
		if (scores.x == 6) {
			return INT_MIN;
		}
		if (scores.y == 6) {
			return INT_MAX;
		}
		scoreMean = scores.y - scores.x;
	}
	else {
		if (scores.x == 6) {
			return INT_MAX;
		}
		if (scores.y == 6) {
			return INT_MIN;
		}
		scoreMean = scores.x - scores.y;
	}
	/*auto extracted = isBlack ? state & MASKS_BLACK : state & MASKS_WHITE;*/
	int midMean = 0;
	int spyMean = 0;
	int hexMean = 0;
	for (auto i = isBlack << 0, j = isBlack ^ 1; i < 122; i += 2, j += 2) {
		if (state[i]) {
			midMean += MIDDLE_H[i >> 1];
			int diff = 0;
			int same = 0;
			for (auto k = 0u; k < 6; ++k) {
				auto adj = logic::MOVE_TABLE[i >> 1][k];
				if (adj == -1) {
					break;
				}
				if (state[(adj << 1) + (isBlack << 0)]) {
					++same;
				}
				if (state[(adj << 1) + (isBlack ^ 1)]) {
					++diff;
				}
			}
			if (diff > 5) {
				++spyMean;
			}
			if (same == 6) {
				++hexMean;
			}

		}
		/*h_end_if:
		if (state[j]) {
			midMean -= MIDDLE_H[j >> 1];
		}*/
	}
	return midMean + spyMean + hexMean + 200 * scoreMean;
}

int automata::basicHeuristic(std::bitset<128U>& state, bool isBlack)
{
	auto scores = logic::getScoreFromState(state);
	int scoreMean = 0;
	if (isBlack) {
		if (scores.x == 6) {
			return INT_MIN;
		}
		if (scores.y == 6) {
			return INT_MAX;
		}
		scoreMean = scores.y * 100 - scores.x * 150;
	}
	else {
		if (scores.x == 6) {
			return INT_MAX;
		}
		if (scores.y == 6) {
			return INT_MIN;
		}
		scoreMean = scores.x * 100 - scores.y * 150;
	}
	/*auto extracted = isBlack ? state & MASKS_BLACK : state & MASKS_WHITE;*/
	int thisMid = 0;
	int thatMid = 0;
	int spyMean = 0;
	int hexMean = 0;
	int adjacency = 0;
	for (auto i = isBlack << 0, j = isBlack ^ 1; i < 122; i += 2, j += 2) {
		if (state[i]) {
			thisMid += MIDDLE_H[i >> 1];
			int diff = 0;
			int same = 0;
			for (auto k = 0u; k < 6; ++k) {
				auto adj = logic::MOVE_TABLE[i >> 1][k];
				if (adj == -1) {
					break;
				}
				if (state[(adj << 1) + (int)isBlack]) {
					++same;
					++adjacency;
				}
				if (state[(adj << 1) + (isBlack ^ 1)]) {
					++diff;
				}
			}
			if (diff > 5) {
				++spyMean;
			}
			if (same == 6) {
				++hexMean;
			}

		}
		if (state[j]) {
			thatMid += MIDDLE_H[j >> 1];
		}
	}
	return (int)(thisMid * 1.1) - thatMid + (adjacency >> 2) + spyMean + hexMean + scoreMean;
}

logic::weightedActionState automata::getBestMove(logic::bitState & state, bool isBlack, unsigned int moveLeft, int timeLeft)
{
	threadPool.wait();
	clock.restart();
	counter = 0;
	if (moveLeft == 0) moveLeft = 4;
	if (timeLeft == 0 || timeLeft > 100) timeLeft = 5;
	timeLeft *= 1000;
	return alphaBeta(state, isBlack, moveLeft, timeLeft);
}

logic::weightedActionState automata::alphaBeta(logic::bitState & state, bool isBlack, unsigned int & moveLeft, int & timeLeft)
{
	auto depth = 2u;
	int miliSec = clock.getElapsedTime().asMilliseconds();
	int lastLayerUsed = 0;
	logic::weightedActionState best;
	int bestV = INT_MIN;
	auto actionStates = logic::getAllValidMoveOrdered(state, isBlack);
	do {
		returned = false;
		auto r = maxTop(actionStates, state, isBlack, depth, moveLeft, timeLeft, INT_MIN, INT_MAX);
		if (r.completed) {
			best = r.bestAs;
			bestV = r.bestV;
		}
		lastLayerUsed = clock.getElapsedTime().asMilliseconds() - miliSec;
		miliSec += lastLayerUsed;

		std::cout << "Depth: " << depth << std::endl;
		std::cout << "Time used: " << lastLayerUsed << std::endl;
		std::cout << "Searched state: " << counter << std::endl;
		std::cout << "bestV: " << bestV << std::endl;
		std::cout << "Best Action: " << best.act.act.count << " " << best.act.act.direction << " " << best.act.act.index << std::endl;

		++depth;
	} while (depth < moveLeft && timeLeft - miliSec >(lastLayerUsed << 3));
	return best;
}

automata::maxTopReturn automata::maxTop(std::multiset<logic::weightedActionState, std::greater<logic::weightedActionState>>& actionStates, logic::bitState & state, bool isBlack, unsigned int depth, unsigned int moveLeft, int & timeLeft, int alpha, int beta)
{
	int bestV = INT_MIN;
	logic::weightedActionState bestAs;
	auto iter = actionStates.begin();
	int threadRemain = threadNumber;
	int completedBranch = 0;
	for (auto i = 0u; iter != actionStates.end(); ++i, ++iter) {
		//take a thread
		--threadRemain;
		threadPool.schedule([this, iter, &bestV, &bestAs, &completedBranch, &threadRemain, isBlack, depth, moveLeft, &alpha, beta]() {
			logic::bitState oneState;
			{
				std::unique_lock<std::mutex> lck{ mtQ };
				if (returned) {
					return;
				}
				oneState = iter->state;
			}
			int curV = minValue(oneState, !isBlack, depth - 1, moveLeft - 1, alpha, beta);
			std::unique_lock<std::mutex> lck{ mtQ };
			if (returned)
				return;

			if (curV > bestV) {
				bestV = curV;
				bestAs = *iter;
			}
			alpha = std::max(alpha, bestV);


			//return a thread
			++threadRemain;
			//increase completed branch
			++completedBranch;
			//notify cv thread made change.
			cv.notify_one();
		});
		//wait for there is remaining thread, if timeout then set cutoff variable then return.
		std::unique_lock<std::mutex> blockLck{ blocker };
		if (!cv.wait_for(blockLck, std::chrono::milliseconds{ timeLeft - clock.getElapsedTime().asMilliseconds() }, [&threadRemain] {return threadRemain > 0; })) {
			std::unique_lock<std::mutex> lck{ mtQ };
			std::cout << "<timeout>" << std::endl;
			returned = true;
			return { bestAs, bestV, false };
		}
	}
	//wait for all branch are completed, if timeout then set cutoff variable then return.
	std::unique_lock<std::mutex> blockLck{ blocker };
	if (!cv.wait_for(blockLck, std::chrono::milliseconds{ timeLeft - clock.getElapsedTime().asMilliseconds() },
		[&completedBranch, &actionStates] {return completedBranch == actionStates.size(); })) {
		std::unique_lock<std::mutex> lck{ mtQ };
		std::cout << "<timeout>" << std::endl;
		returned = true;
		return { bestAs, bestV, false };
	}


	std::unique_lock<std::mutex> lck{ mtQ };
	std::cout << "<completed>" << std::endl;
	returned = true;

	return { bestAs, bestV, true };
}

int automata::maxValue(logic::bitState & state, bool isBlack, unsigned int depth, unsigned int moveLeft, int alpha, int beta)
{
	if (returned)
		return INT_MAX;
	++counter;
	auto blackLoss = (int)(state._2 >> 58 & 7);
	auto whiteLoss = (int)(state._2 >> 61 & 7);
	if (depth < 1 || moveLeft < 1 || whiteLoss > 5 || blackLoss > 5) {
		return hn(state, isBlack, blackLoss, whiteLoss);
	}
	auto actionStates = logic::getAllValidMoveOrdered(state, isBlack);
	int bestV = INT_MIN;
	for (auto as : actionStates) {
		bestV = std::max(bestV, minValue(as.state, !isBlack, depth - 1, moveLeft - 1, alpha, beta));
		if (bestV >= beta)
			return bestV;
		alpha = std::max(alpha, bestV);
	}
	return bestV;
}

int automata::minValue(logic::bitState & state, bool isBlack, unsigned int depth, unsigned int moveLeft, int alpha, int beta)
{
	if (returned)
		return INT_MIN;
	++counter;
	auto blackLoss = (int)(state._2 >> 58 & 7);
	auto whiteLoss = (int)(state._2 >> 61 & 7);
	if (depth < 1 || moveLeft < 1 || whiteLoss > 5 || blackLoss > 5) {
		return hn(state, !isBlack, blackLoss, whiteLoss);
	}
	auto actionStates = logic::getAllValidMoveOrdered(state, isBlack);
	int bestV = INT_MAX;
	for (auto as : actionStates) {
		bestV = std::min(bestV, maxValue(as.state, !isBlack, depth - 1, moveLeft - 1, alpha, beta));
		if (bestV <= alpha)
			return bestV;
		beta = std::min(beta, bestV);
	}

	return bestV;
}

int automata::h1(logic::bitState & state, bool isBlack, int& blackLost, int& whiteLost)
{
	int scoreMean = 0;
	if (isBlack) {
		if (blackLost == 6) {
			return INT_MIN;
		}
		if (whiteLost == 6) {
			return INT_MAX;
		}
		scoreMean = whiteLost * 100 - blackLost * 150;
	}
	else {
		if (whiteLost == 6) {
			return INT_MAX;
		}
		if (blackLost == 6) {
			return INT_MIN;
		}
		scoreMean = blackLost * 100 - whiteLost * 150;
	}
	int thisMid = 0;
	int thatMid = 0;
	int spyMean = 0;
	int hexMean = 0;
	int adjacency = 0;
	for (auto i = isBlack << 0, j = isBlack ^ 1; i < 122; i += 2, j += 2) {
		if ((i & 64 ? (bool)(state._2 & 1ull << (i & 63)) : (bool)(state._1 & 1ull << i))) {
			thisMid += MIDDLE_H[i >> 1];
			int diff = 0;
			int same = 0;
			for (auto k = 0u; k < 6; ++k) {
				auto adj = logic::MOVE_TABLE[i >> 1][k];
				if (adj == -1) {
					break;
				}
				if ((adj & 32 ? (bool)(state._2 & 1ull << ((adj & 31) << 1) + isBlack) : (bool)(state._1 & 1ull << (i << 1) + isBlack))) {
					++same;
					++adjacency;
				}
				if ((adj & 32 ? (bool)(state._2 & 1ull << ((adj & 31) << 1) + (isBlack ^ 1)) : (bool)(state._1 & 1ull << (i << 1) + (isBlack ^ 1)))) {
					++diff;
				}
			}
			if (diff > 5) {
				++spyMean;
			}
			if (same == 6) {
				++hexMean;
			}

		}
		if ((j & 64 ? (bool)(state._2 & 1ull << (j & 63)) : (bool)(state._1 & 1ull << j))) {
			thatMid += MIDDLE_H[j >> 1];
		}
	}
	return (int)(thisMid * 1.1) - thatMid + (adjacency >> 2) + spyMean + hexMean + scoreMean;
}

int automata::h2(logic::bitState &, bool isBlack, int& blackLost, int& whiteLost)
{
	return 0;
}

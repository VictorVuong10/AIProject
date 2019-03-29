#include "logic.h"

bool isWhitePiece(int index, std::bitset<128U>& state) {
	return state[index << 1];
}

bool isBlackPiece(int index, std::bitset<128U>& state) {
	return state[(index << 1) + 1];
}


bool isEmpty(int index, std::bitset<128U>& state) {
	//im changing this because 0 || 0 will return false so but then that is empty so it should return true
	// 0 is false and 1 is true;
	return !state[index << 1] && !state[(index << 1) + 1];
}

std::bitset<128U>& logic::sideMove(std::bitset<128U>& state, action& act, bool isBlackTurn) {
	int marbleCount = act.count;
	int index = act.index;
	int direction = act.direction;

	//counter clockwise first
	int drr1 = (direction + 5) % 6;

	//counter clockwise second
	int drr2 = (direction + 4) % 6;
	int drr = drr1;

	int next = -1;
	for (int i = 0; i < marbleCount; ++i) {

		if (index == -1)
			return state;

		next = MOVE_TABLE[index][direction];
		
		auto indexState = state[index << 1] << 1 | state[(index << 1) + 1] << 0;

		//index slot is empty, maybe we can change direction
		if (!indexState) {
			//try to change direction
			if (i == 1) {
				drr = drr2;
				next = index;
				int drrOpp = (direction + 3) % 6;
				index = MOVE_TABLE[index][drrOpp];
				//the next direction out bound, nothing to move, both direction failed
				if (index == -1)
					return state;
				indexState = state[index << 1] << 1 | state[(index << 1) + 1] << 0;;
				//next direction is also emppty or different color, both direction failed
				if (!indexState || (indexState & 1 ^ isBlackTurn << 0))
					return state;
				//same color
				else {
					index = MOVE_TABLE[index][drr];
					
					continue;
				}
			}
			return state;
		}
		//different color
		if (indexState & 1 ^ isBlackTurn << 0)
			return state;

		//not allow suicide
		if (next == -1)
			return state;

		//not empty
		if (state[next << 1] || state[(next << 1) + 1])
			return state;

		index = MOVE_TABLE[index][drr];
	}

	index = act.index;
	next = MOVE_TABLE[index][direction];
	for (int i = 0; i < marbleCount; ++i) {
		state.set(index << 1, 0);
		state.set((index << 1) + 1, 0);
		state.set(next << 1, !isBlackTurn);
		state.set((next << 1) + 1, isBlackTurn);
		index = MOVE_TABLE[index][drr];
		next = MOVE_TABLE[index][direction];
	}
	return state;
}

std::bitset<128U>& logic::inlineMove(std::bitset<128U>& state, action& act, bool isBlackTurn) {
	int marbleCount = act.count;
	int index = act.index;
	int direction = act.direction;

	int sameColorCount = 0;
	int endIndex = 0;
	for (int next = index; sameColorCount < 4; ++sameColorCount, next = MOVE_TABLE[next][direction]) {
		//suicide case, directly return to disable suicide
		if (next == -1) {
			/*int resetI = 127 - (index << 1);
			state.set(resetI, 0);
			state.set(resetI - 1, 0);*/
			return state;
		}
		auto nextState = state[next << 1] << 1 | state[(next << 1) + 1] << 0;
		//empty maybe (valid)
		if (!nextState) {
			//not moving anything(invalid)
			if (sameColorCount == 0)
				return state;
			//create and return new state
			/*std::bitset<128U> mask{ 0 };
			std::bitset<128U> setMask{ 0 };
			for (int c = 0, mNext = index, nNext = MOVE_TABLE[mNext][direction]; c < sameColorCount; ++c, mNext = nNext, nNext = MOVE_TABLE[nNext][direction]) {
				std::bitset<128U> subMask{ 3 };
				int patternI = 127 - (index << 1);
				auto p = state[127 - patternI] << 1 | state[patternI - 1] << 0;
				std::bitset<128U> pattern(p);
				mask |= subMask << (125 - (mNext << 1));
				setMask |= pattern << (125 - (nNext << 1));
			}
			state &= ~mask;
			state |= setMask;
			return state;*/
			state.set(index << 1, 0);
			state.set((index << 1) + 1, 0);
			state.set(next << 1, !isBlackTurn);
			state.set((next << 1) + 1, isBlackTurn);
			return state;
		}
		//different color(check further)
		if (nextState & 1 ^ isBlackTurn << 0) {
			//different color but can't push(invalid)
			if (sameColorCount < 2) {
				return state;
			}
			break;
		}
		endIndex = next;
	}
	//no marble or more than 3 marble (invalid)
	if (!sameColorCount || sameColorCount > 3)
		return state;

	int oppIndex = MOVE_TABLE[endIndex][direction];
	int oppColorCount = 0;
	int oppEndIndex = 0;
	//The only case we enter this loop is that we met a different color
	for (int next = oppIndex; oppColorCount < sameColorCount; ++oppColorCount, next = MOVE_TABLE[next][direction]) {
		//KILL THE OPPONENT
		if (next == -1) {
			state.set(index << 1, 0);
			state.set((index << 1) + 1, 0);
			state.set(oppIndex << 1, !isBlackTurn);
			state.set((oppIndex << 1) + 1, isBlackTurn);

			//++score
			auto scoreI = (isBlackTurn << 0) * 3 + 122;
			auto max = scoreI + 3;
			bool val = state[scoreI];
			bool carry = val & 1;
			state.set(scoreI, val ^ 1);
			while (carry && scoreI < max) {
				val = state[++scoreI];
				state.set(scoreI, val ^ carry);
				carry &= val;
			}

			return state;
		}
		auto nextState = state[next << 1] << 1 | state[(next << 1) + 1] << 0;
		//empty(valid)
		if (!nextState) {
			//create and return new state
			state.set(next << 1, isBlackTurn);
			state.set((next << 1) + 1, !isBlackTurn);
			state.set(index << 1, 0);
			state.set((index << 1) + 1, 0);
			state.set(oppIndex << 1, !isBlackTurn);
			state.set((oppIndex << 1) + 1, isBlackTurn);
			return state;
		}
		//meet same color again(invalid)
		if ((nextState & 2) >> 1 ^ isBlackTurn << 0) {
			return state;
		}
		oppEndIndex = next;
	}
	//opponent has more, can't push(invalid)
	return state;

}

std::bitset<128U> logic::move(std::bitset<128U> state, action act, bool isBlackTurn) {
	return act.count == 1? inlineMove(state, act, isBlackTurn) : sideMove(state, act, isBlackTurn);
}


std::vector<std::pair<logic::action, std::bitset<128U>>> logic::getAllValidMove(std::bitset<128U> state, bool isBlackTurn) {
	std::vector<std::pair<logic::action, std::bitset<128U>>> action_state{};

	//count 1-3, index 0-60, direction 0-5
	for (auto bitindex = isBlackTurn << 0; bitindex < 122; bitindex += 2) {
		if (state[bitindex]) {
			for (int direction = 0; direction < 6; ++direction) {
				for (int count = 1; count < 4; ++count) {
					action act{ count, (bitindex - (isBlackTurn << 0)) >> 1, direction };
					auto nstate = move(state, act, isBlackTurn);
					if (nstate != state) {
						action_state.push_back({ act, nstate });
					}
				}
			}
		}
	}
	return action_state;
}


sf::Vector2u logic::getScoreFromState(std::bitset<128U>& state) {
	auto whiteLost = 0u, blackLost = 0u;
	for (auto i = 124u, j = 127u; i > 121; --i, --j) {
		whiteLost <<= 1;
		blackLost <<= 1;
		whiteLost |= state[j] << 0;
		blackLost |= state[i] << 0;
	}
	return { blackLost, whiteLost };
}
#include "logic.h"



logic::logic()
{
}


logic::~logic()
{
}

bool isWhitePiece(int index, std::bitset<128U>& state) {
	return state[127 - (index << 1)];
}

bool isBlackPiece(int index, std::bitset<128U>& state) {
	return state[127 - (index << 1) - 1];
}


bool isEmpty(int index, std::bitset<128U>& state) {
	int bitIndex = 127 - (index << 1);
	//im changing this because 0 || 0 will return false so but then that is empty so it should return true
	// 0 is false and 1 is true;
	return !state[bitIndex - 1] && !state[bitIndex];
}



std::bitset<128U> logic::sideMove(std::bitset<128U> state, action act, bool isBlackTurn) {

	int marbleCount = act.count;
	int index = act.index;
	int direction = act.direction;

	int checkDirection = (direction + 4) % 6;
	int secondCheckDirection = (direction + 5) % 6;

	int storedIndex = index;

	//if (MOVE_TABLE[index][direction] == -1) {

	//checkDirection = to check what position the marbles are in i.e 3 marbles in a line downrightwards or in a line downleftward
//	int checkDirection = 4;

	for (int i = 0; i < marbleCount; i++) {

		int checkIndex = MOVE_TABLE[index][direction];
		//4 = downRight

		//if (marbleCount > 0) {
			//checkIndex =
		//}

		//probably should add a helper method for isBlack piece, is white peice, and is null
		//that takes in 2 ints and checks the state to see if it is ... peice


		if (!isEmpty(checkIndex, state)) {
			//i think 01 = black

			if (i == 1
				&& ((isBlackTurn && isBlackPiece(checkIndex, state))
					|| (!isBlackTurn && isWhitePiece(checkIndex, state)))
				) {
				checkDirection = secondCheckDirection;
				//index of the 2nd marble 
				index = MOVE_TABLE[storedIndex][checkDirection];


				checkIndex = MOVE_TABLE[index][direction];
				if (!isEmpty(checkIndex, state)) {
					return state;
				}

			} else {
				return state;
			}

		}

		//just so index doesnt become -1 but i dont think this matters?
		if (i != marbleCount - 1)
			index = MOVE_TABLE[index][checkDirection];

	}

	//if it exist after for loop then it is validated

	std::bitset<128U> newState = state;

	if (isBlackTurn) {
		//instead of first index i could go backwards but i dont really want to atm,
		for (int i = 0; i < marbleCount; i++) {
			newState.set((127 - storedIndex * 2) -1 , 0);
			newState.set((127 - storedIndex * 2) + 0, 0);
			newState.set(127- ((MOVE_TABLE[storedIndex][direction]) * 2) -1 , 1);
			newState.set(127 -((MOVE_TABLE[storedIndex][direction]) * 2) + 0, 0);

			//just so index does not become -1 but i dont think this matters.
			if (i < marbleCount - 1)
				storedIndex = MOVE_TABLE[storedIndex][checkDirection];
		}
	}
	else {
		for (int i = 0; i < marbleCount; i++) {
			newState.set((127 - storedIndex * 2) - 1, 0);
			newState.set((127 - storedIndex * 2) + 0, 0);
			newState.set(127 - ((MOVE_TABLE[storedIndex][direction]) * 2) - 1, 0);
			newState.set(127 - ((MOVE_TABLE[storedIndex][direction]) * 2) + 0, 1);

			if (i < marbleCount - 1)
				storedIndex = MOVE_TABLE[storedIndex][checkDirection];

		}
	}


	return newState;


}

std::bitset<128U> logic::inlineMove(std::bitset<128U> state, action act, bool isBlackTurn) {
	int marbleCount = act.count;
	int index = act.index;
	int direction = act.direction;

	int sameColorCount = 0;
	int endIndex = 0;
	for (int next = index; sameColorCount < 4; ++sameColorCount, next = MOVE_TABLE[next][direction]) {
		//out bound(invalid)
		if (next == -1)
			return state;
		int biti = 127 - (next << 1);
		auto nextState = state[biti] << 1 | state[biti - 1] << 0;
		//empty(valid)
		if (!nextState) {
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
			int resetI = 127 - (index << 1);
			int setI = 127 - (next << 1);
			state.set(resetI, 0);
			state.set(resetI - 1, 0);
			state.set(setI, !isBlackTurn);
			state.set(setI - 1, isBlackTurn);
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
	//no current turn marble(invalid)
	if (!sameColorCount)
		return state;

	int oppIndex = MOVE_TABLE[endIndex][direction];
	int oppColorCount = 0;
	int oppEndIndex = 0;
	for (int next = oppIndex; oppColorCount < sameColorCount; ++oppColorCount, next = MOVE_TABLE[next][direction]) {
		if (next == -1) {
			//pushing self out(invalid)
			if(oppColorCount < 1)
				return state;
			else { //KILL THE OPPONENT
				int resetOpp = 127 - (oppIndex << 1);
				int resetTeam = 127 - (index << 1);
				int setTeam = 127 - (oppIndex << 1);
				state.set(resetOpp, 0);
				state.set(resetOpp - 1, 0);
				state.set(resetTeam, 0);
				state.set(resetTeam - 1, 0);
				state.set(setTeam, !isBlackTurn);
				state.set(setTeam - 1, isBlackTurn);


				return state;
			}
		}
		int biti = 127 - (next << 1);
		auto nextState = state[biti] << 1 | state[biti - 1] << 0;
		//empty(valid)
		if (!nextState) {
			//create and return new state
			int resetOpp = 127 - (oppIndex << 1);
			int resetTeam = 127 - (index << 1);
			int setOpp = 127 - (next << 1);
			int setTeam = 127 - (oppIndex << 1);
			state.set(resetOpp, 0);
			state.set(resetOpp - 1, 0);
			state.set(setOpp, isBlackTurn);
			state.set(setOpp - 1, !isBlackTurn);
			state.set(resetTeam, 0);
			state.set(resetTeam - 1, 0);
			state.set(setTeam, !isBlackTurn);
			state.set(setTeam - 1, isBlackTurn);
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
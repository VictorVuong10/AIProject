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


	//i dont think we need this part, just put the direction the rhs directyl into checkDirections?
	int drr1 = (direction + 4) % 6;
	int drr2 = (direction + 5) % 6;

	int checkDirection = drr1;
	int secondCheckDirection = drr2;

	int storedIndex = index;

	std::cout << "Check Direction :" << checkDirection << std::endl;


	std::cout << "Second Check Direction :" << secondCheckDirection << std::endl;

	std::cout << "index :" << index << std::endl;

	std::cout << "state :\n" << state << std::endl;

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


		std::cout << "checkIndex: " << checkIndex << "\n";

		std::cout << "STATEcheckIndex: " << state[(checkIndex * 2) + 0] << "\n";

		std::cout << "STATEcheckIndex: " << state[(checkIndex * 2) + 1] << "\n";

		std::wcout << "a fucking bool" << (state[(checkIndex * 2) + 0] != 0 && state[(checkIndex * 2) + 1] != 0) << "\n";

		std::wcout << "a fucking bool first bit :" << (state[(checkIndex * 2) + 0] != 0) << "\n";

		std::wcout << "a fucking bool second bit :" << (state[(checkIndex * 2) + 1] != 0) << "\n";

		std::cout << i << std::endl;


		if (!isEmpty(checkIndex, state)) {
			//i think 01 = black
			std::cout << "checkIndex: " << checkIndex << "\n";
			std::cout << "checkIndex is empty: " << isEmpty(checkIndex, state) << "\n";


			if (i == 1) {
				std::cout << "i is = 1";
			}

			if (isBlackTurn) {
				std::cout << "is black turn \n";
			}

			if (isBlackPiece(checkIndex, state)) {
				std::cout << "checkIndex of if statement" << checkIndex << "\n";
				std::cout << "is it a blackpiece for that check index?, yes yes it is.";
			}

			if (i == 1
				&& ((isBlackTurn && isBlackPiece(checkIndex, state))
					|| (!isBlackTurn && isWhitePiece(checkIndex, state)))
				) {
				checkDirection = secondCheckDirection;
				//index of the 2nd marble 
				index = MOVE_TABLE[storedIndex][checkDirection];

				std::cout << "yes";

				checkIndex = MOVE_TABLE[index][direction];
				if (!isEmpty) {
					return state;
				}

				std::cout << "good";
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

	//int initialIndex = index;

	//if (isBlackTurn) {

	//	int blackMarbleCount = 0;

	//	//index is the index to check

	//	while (isBlackPiece(index, state)) {
	//		blackMarbleCount++;
	//		index = MOVE_TABLE[index][direction];
	//	}

	//	


	//	if (blackMarbleCount > 3) {
	//		return state;
	//	}

	//	if (isEmpty(index, state)) {
	//		std::bitset<128U> newState = state;

	//		newState.set((initialIndex * 2) + 0, 0);
	//		newState.set((initialIndex * 2) + 1, 0);
	//		newState.set((index * 2) + 0, 0);
	//		newState.set((index * 2) + 1, 1);

	//		return newState;
	//	}

	//	//stil have to do the part where the marbles push each other.


	//}
	//else {
	//	//white scenario

	//	int whiteMarbleCount = 0;

	//	//index is the index to check

	//	while (isBlackPiece(index, state)) {
	//		whiteMarbleCount++;
	//		index = MOVE_TABLE[index][direction];
	//	}

	//	if (whiteMarbleCount > 3) {
	//		return state;
	//	}

	//	if (isEmpty(index, state)) {
	//		std::bitset<128U> newState = state;

	//		newState.set((initialIndex * 2) + 0, 0);
	//		newState.set((initialIndex * 2) + 1, 0);
	//		newState.set((index * 2) + 0, 1);
	//		newState.set((index * 2) + 1, 0);

	//		return newState;
	//	}

	//	//pushing case here

	//}
}

std::bitset<128U> logic::move(std::bitset<128U> state, action act, bool isBlackTurn) {
	
	std::cout << act.count << " asdasd : " << act.index << "asdasd \n";

	if (act.count == 1) {

		return inlineMove(state, act, isBlackTurn);

	}

	else {

		
		return sideMove(state, act, isBlackTurn);

		//if (direction == 0) {

		//	//4 is the check direction, 5 is the second possible check direction
		//	return moveHelper(marbleCount, index, direction, 4, 5);

		//}
		//else if (direction == 1) {

		//	//4 is the check direction, 5 is the second possible check direction
		//	return moveHelper(marbleCount, index, direction, 5, 1);

		//}
		//else if (direction == 2) {

		//	//4 is the check direction, 5 is the second possible check direction
		//	return moveHelper(marbleCount, index, direction, 4, 3);

		//}
		//else if (direction == 3) {

		//	//4 is the check direction, 5 is the second possible check direction
		//	return moveHelper(marbleCount, index, direction, 5, 4);

		//}
		//else if (direction == 4) {

		//	//4 is the check direction, 5 is the second possible check direction
		//	return moveHelper(marbleCount, index, direction, 2, 3);

		//}
		////direction == 5 i probably shoulda done a switch statement but screw it.
		//return moveHelper(marbleCount, index, direction, 3, 4);

	}


}
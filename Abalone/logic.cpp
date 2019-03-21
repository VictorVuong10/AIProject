#include "logic.h"



logic::logic()
{
}


logic::~logic()
{
}

bool isBlackPiece(int index, std::bitset<128U>& state) {
	return state[127 - (index << 1)];
}

bool isWhitePiece(int index, std::bitset<128U>& state) {
	return state[127 - (index << 1) - 1];;
}

bool isEmpty(int index, std::bitset<128U>& state) {
	int bitIndex = 127 - (index << 1);
	return state[bitIndex] || state[bitIndex - 1];
}



std::bitset<128U> logic::sideMove(std::bitset<128U> state, action act, bool isBlackTurn) {

	int marbleCount = act.count;
	int index = act.index;
	int direction = act.direction;

	int drr1 = (direction + 5) % 6;
	int drr2 = (direction + 4) % 6;
	int drr = drr1;

	int next = MOVE_TABLE[index][direction];
	//out bound
	if (next == -1)
		return state;

	int nextBitIndex = 127 - (next << 1);
	//can't move anyway when index is blocked
	if (state[nextBitIndex] || state[nextBitIndex - 1]) {
		return false;
	}

	int drr1Index = MOVE_TABLE[index][drr1];
	int drr1BitIndex = 127 - (drr1Index << 1);
	auto drr1State = state[drr1BitIndex] << 1 | state[drr1BitIndex - 1] << 0;

	int secondIndex = drr1Index;
	int secondBitIndex = drr1BitIndex;
	
	//not empty, maybe marbles align direction 1
	if (drr1State) {
		//different color, both directions will be blocked
		if (drr1State & 1 ^ isBlackTurn << 0)
			return state;

		//same color, comfirmed moving direction 1
		int drr1nextIndex = MOVE_TABLE[drr1Index][drr1];
		int drr1nextBitIndex = 127 - (drr1nextIndex << 1);
		//but blocked
		if(state[drr1nextBitIndex] || state[drr1nextBitIndex - 1]){
			return state;
		}

		//side move 2 marble is valid anyway, check 3

	}
	//maybe moving towward direction 2
	else {
		int drr2Index = MOVE_TABLE[index][drr2];
		int drr2BitIndex = 127 - (drr2Index << 1);
		auto drr2NextState = state[drr2BitIndex] << 1 | state[drr2BitIndex - 1] << 0;
		//different color, both directions are blocked
		if (drr2NextState & 1 ^ isBlackTurn << 0)
			return state;

		//second direction is checked, so moving 2 is valid, check 3
		drr = drr2;
		secondIndex = drr2Index;
		secondBitIndex = drr2BitIndex;

	}

	bool move3 = false;

	int last = MOVE_TABLE[secondIndex][drr];
	int lastBitIndex = 127 - (last << 1);
	auto lastState = state[lastBitIndex] << 1 | state[lastBitIndex - 1] << 0;

	//same color third
	if (lastState & 2 ^ isBlackTurn << 1) {
		//check third next
		int lastNextIndex = MOVE_TABLE[last][drr];
		int lastNextBitIndex = 127 - (lastNextIndex << 1);
		//empty(valid), move 3 marble toward direction
		if (!(state[lastNextBitIndex] || state[lastNextBitIndex - 1])) {
			move3 = true;
		}
		//blocked, move valid 2
	}
	//empty 3rd or different color third found, move the valid 2

	state.set(127 - (index << 1), 0);
	state.set(127 - (index << 1) - 1, 0);
	state.set(secondBitIndex, 0);
	state.set(secondBitIndex - 1, 0);
	if (move3) {
		state.set(lastBitIndex, 0);
		state.set(lastBitIndex - 1, 0);
	}
	return state;


//	//---------------------------------------------------------------------------
//
//	int checkDirection = drr1;
//	int secondCheckDirection = drr2;
//
//	int storedIndex = index;
//
//	//if (MOVE_TABLE[index][direction] == -1) {
//
//	//checkDirection = to check what position the marbles are in i.e 3 marbles in a line downrightwards or in a line downleftward
////	int checkDirection = 4;
//
//	for (int i = 0; i < marbleCount; i++) {
//
//		int checkIndex = MOVE_TABLE[index][direction];
//		//4 = downRight
//
//		//if (marbleCount > 0) {
//			//checkIndex =
//		//}
//
//		//probably should add a helper method for isBlack piece, is white peice, and is null
//		//that takes in 2 ints and checks the state to see if it is ... peice
//
//
//		std::cout << "checkIndex: " << checkIndex << "\n";
//
//		std::cout << "STATEcheckIndex: " << state[(checkIndex * 2) + 0] << "\n";
//
//		std::cout << "STATEcheckIndex: " << state[(checkIndex * 2) + 1] << "\n";
//
//		std::wcout << "a fucking bool" << (state[(checkIndex * 2) + 0] != 0 && state[(checkIndex * 2) + 1] != 0) << "\n";
//
//		std::wcout << "a fucking bool first bit :" << (state[(checkIndex * 2) + 0] != 0) << "\n";
//
//		std::wcout << "a fucking bool second bit :" << (state[(checkIndex * 2) + 1] != 0) << "\n";
//
//		if (!isEmpty) {
//			//i think 01 = black
//			std::cout << "yes2.0";
//
//
//			if (i == 1
//				&& ((isBlackTurn && isBlackPiece(checkIndex, state))
//					|| (!isBlackTurn && isWhitePiece(checkIndex, state)))
//				) {
//				checkDirection = secondCheckDirection;
//				//index of the 2nd marble 
//				index = MOVE_TABLE[storedIndex][checkDirection];
//
//				std::cout << "yes";
//
//				checkIndex = MOVE_TABLE[index][direction];
//				if (!isEmpty) {
//					return state;
//				}
//
//				std::cout << "good";
//			}
//
//			//didnt calculate white scenario yet.
//
//			else {
//				return state;
//			}
//
//		}
//
//		//just so index doesnt become -1 but i dont think this matters?
//		if (i != marbleCount - 1)
//			index = MOVE_TABLE[index][checkDirection];
//
//	}
//
//	//if it exist after for loop then it is validated
//
//	std::bitset<128U> newState = state;
//
//	if (isBlackTurn) {
//		//instead of first index i could go backwards but i dont really want to atm,
//		for (int i = 0; i < marbleCount; i++) {
//			newState.set((127 - storedIndex * 2) + 0, 0);
//			newState.set((127 - storedIndex * 2) + 1, 0);
//			newState.set(((MOVE_TABLE[storedIndex][direction]) * 2) + 0, 0);
//			newState.set(((MOVE_TABLE[storedIndex][direction]) * 2) + 1, 1);
//
//			//just so index does not become -1 but i dont think this matters.
//			if (i < marbleCount - 1)
//				storedIndex = MOVE_TABLE[storedIndex][checkDirection];
//		}
//	}
//	else {
//		for (int i = 0; i < marbleCount; i++) {
//			newState.set((127 - storedIndex * 2) + 0, 0);
//			newState.set((127 - storedIndex * 2) + 1, 0);
//			newState.set(((MOVE_TABLE[storedIndex][direction]) * 2) + 0, 1);
//			newState.set(((MOVE_TABLE[storedIndex][direction]) * 2) + 1, 0);
//
//			if (i < marbleCount - 1)
//				storedIndex = MOVE_TABLE[storedIndex][checkDirection];
//
//		}
//	}
//
//
	//return newState;


}

//{
//int marbleCount = act.count;
//int index = act.index;
//int direction = act.direction;
//
//int drr1 = (direction + 5) % 6;
//int drr2 = (direction + 4) % 6;
//int drr = drr1;
//
//int next = MOVE_TABLE[index][direction];
////out bound
//if (next == -1)
//return state;
//
//int nextBitIndex = 127 - (next << 1);
////can't move anyway when index is blocked
//if (state[nextBitIndex] || state[nextBitIndex - 1]) {
//	return false;
//}
//
//int drr1Index = MOVE_TABLE[index][drr1];
//int drr1BitIndex = 127 - (drr1Index << 1);
//auto drr1State = state[drr1BitIndex] << 1 | state[drr1BitIndex - 1] << 0;
//
//int secondIndex = drr1Index;
//int secondBitIndex = drr1BitIndex;
//
////not empty, maybe marbles align direction 1
//if (drr1State) {
//	//different color, both directions will be blocked
//	if (drr1State & 1 ^ isBlackTurn << 0)
//		return state;
//
//	//same color, comfirmed moving direction 1
//	int drr1nextIndex = MOVE_TABLE[drr1Index][drr1];
//	int drr1nextBitIndex = 127 - (drr1nextIndex << 1);
//	//but blocked
//	if (state[drr1nextBitIndex] || state[drr1nextBitIndex - 1]) {
//		return state;
//	}
//
//	//side move 2 marble is valid anyway, check 3
//
//}
////maybe moving towward direction 2
//else {
//	int drr2Index = MOVE_TABLE[index][drr2];
//	int drr2BitIndex = 127 - (drr2Index << 1);
//	auto drr2NextState = state[drr2BitIndex] << 1 | state[drr2BitIndex - 1] << 0;
//	//different color, both directions are blocked
//	if (drr2NextState & 1 ^ isBlackTurn << 0)
//		return state;
//
//	//second direction is checked, so moving 2 is valid, check 3
//	drr = drr2;
//	secondIndex = drr2Index;
//	secondBitIndex = drr2BitIndex;
//
//}
//
//bool move3 = false;
//
//int last = MOVE_TABLE[secondIndex][drr];
//int lastBitIndex = 127 - (last << 1);
//auto lastState = state[lastBitIndex] << 1 | state[lastBitIndex - 1] << 0;
//
////same color third
//if (lastState & 2 ^ isBlackTurn << 1) {
//	//check third next
//	int lastNextIndex = MOVE_TABLE[last][drr];
//	int lastNextBitIndex = 127 - (lastNextIndex << 1);
//	//empty(valid), move 3 marble toward direction
//	if (!(state[lastNextBitIndex] || state[lastNextBitIndex - 1])) {
//		move3 = true;
//	}
//	//blocked, move valid 2
//}
////empty 3rd or different color third found, move the valid 2
//
//state.set(127 - (index << 1), 0);
//state.set(127 - (index << 1) - 1, 0);
//state.set(secondBitIndex, 0);
//state.set(secondBitIndex - 1, 0);
//if (move3) {
//	state.set(lastBitIndex, 0);
//	state.set(lastBitIndex - 1, 0);
//}
//return state;
//}

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
	/*if (!sameColorCount)
		return state;*/

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
		if (nextState & 2 ^ isBlackTurn << 1) {
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
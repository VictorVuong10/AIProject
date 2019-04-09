#pragma once

#include "gui.h"
#include "testcaseManager.h"
#include "logic.h"
#include "game.h"
#include <vector>


int main() {

	int testcaseNum;
	std::cout << "please enter how many test cases you want to run: " << std::endl;
	std::cin >> testcaseNum;
	testcaseManager::runAllTestcasesCompare(testcaseNum);

	gui g{};
	g.start();

	/*using namespace std;

	bitset<128U> s{ game::STANDARD_STR };
	auto v = logic::getAllValidMove(s, 1);

	cout << v.size() << endl;
	for (auto r : v) {
		cout << r.first.count << " " << r.first.index << " " << r.first.direction << endl;
	}*/

	/*int dummy;
	std::cin >> dummy;*/
	return 0;
}


//void test(std::bitset<128U> state, bool isBlackTurn) {
//	int counter = 0;
//	std::vector<logic::weightedAction> list;
//	for (auto bitindex = isBlackTurn << 0; bitindex < 122; bitindex += 2) {
//		if (state[bitindex]) {
//			for (int direction = 0; direction < 6; ++direction) {
//				for (int count = 1; count < 4; ++count) {
//					logic::weightedAction wAct{ {count, (bitindex - (isBlackTurn << 0)) >> 1, direction}, 0, 0, -1 };
//					if (logic::isValidMove_ai(state, wAct, isBlackTurn)) {
//						++counter;
//						list.push_back(std::move(wAct));
//					}
//				}
//			}
//		}
//	}
//	auto listCorrect = logic::getAllValidMove(state, isBlackTurn);
//	std::vector<logic::action> listNotIn;
//
//	for (auto actionState : listCorrect) {
//		bool added = false;
//		for (auto i = list.begin(); i != list.end(); ++i) {
//			if (i->act == actionState.first) {
//				list.erase(i);
//				added = true;
//				break;
//			}
//		}
//		if(!added)
//			listNotIn.push_back(actionState.first);
//	}
//	std::cout << "EXCEED: " << std::endl;
//	for (auto a : list) {
//		std::cout << a.act.count << " " << a.act.direction << " " << a.act.index << std::endl;
//	}
//	std::cout << "NOT IN: " << std::endl;
//	for (auto a : listNotIn) {
//		std::cout << a.count << " " << a.direction << " " << a.index << std::endl;
//	}
//	std::cout << std::endl;
//}
//
//int main() {
//	std::bitset<128U> asd{ "00000000000000000101000010100101010010101000010100001010000000100000000000000010100000010100001010000101011010000001010000000000" };
//	/*for (int i = 1; i <= 16; ++i) {
//		std::string path = "../Test/Test" + std::to_string(i) + ".input";
//		bool isBlackTurn = true;
//		auto state = testcaseManager::notationToState(path, &isBlackTurn);
//		std::cout << testcaseManager::printState(state[0]) << std::endl;
//		test(state[0], isBlackTurn);
//	}*/
//	test(asd, false);
//
//	int j;
//	std::cin >> j;
//	return 0;
//}


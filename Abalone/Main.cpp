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
	testcaseManager::runAllTestcasesCompareNew(testcaseNum);

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
//
//struct _2b{
//	union {
//		struct {
//			unsigned long long _2;
//			unsigned long long _1;
//			
//		};
//	};
//	/*inline bool operator[](const unsigned int & i) {
//		return i < 64 ? ((_1 >> i) & 1) : ((_2 >> (i & 63)) & 1);
//	}*/
//	inline bool operator[](const unsigned int & i) {
//		if(i < 64)
//			return _1 >> i & 1;
//		return _2 >> (i & 63) & 1;
//	}
//};
//
//int main() {
//	sf::Clock timer;
//	std::bitset<128U> bs{"00000001010010100101011010100001010010100000000000000000000000000000000000000000000000000000001010000101001010100101011010000101"};
//	_2b bits;
//	bits._1 = 0b0000000000000000000000000000001010000101001010100101011010000101;
//	bits._2 = 0b0000000101001010010101101010000101001010000000000000000000000000;
//
//	for (int i = 0; i < 128; ++i) {
//		if (bits[i] != bs[i]) {
//			std::cout << "something wrong with struct" << std::endl;
//		}
//	}
//
//
//	timer.restart();
//	for (int i = 0; i < 100000; ++i) {
//		if (66 < 64)
//			bits._1 >> 66 & 1;
//		bits._2 >> (66 & 63) & 1;
//	}
//	std::cout << timer.getElapsedTime().asMicroseconds() << std::endl;
//
//	timer.restart();
//	for (int i = 0; i < 100000; ++i) {
//		bool b = bs[66];
//	}
//	std::cout << timer.getElapsedTime().asMicroseconds() << std::endl;
//
//	int j = 0;
//	std::cin >> j;
//	return 0;
//}


void test(logic::bitState state, std::bitset<128U> bitsetState, bool isBlackTurn) {
	int counter = 0;
	auto list = logic::getAllValidMoveOrdered(state, isBlackTurn);

	auto listCorrect = logic::getAllValidMove(bitsetState, isBlackTurn);
	std::vector<logic::action> listNotIn;

	for (auto actionState : listCorrect) {
		bool added = false;
		for (auto i = list.begin(); i != list.end(); ++i) {
			if (i->act.act == actionState.first) {
				list.erase(i);
				added = true;
				break;
			}
		}
		if(!added)
			listNotIn.push_back(actionState.first);
	}
	std::cout << "EXCEED: " << std::endl;
	for (auto a : list) {
		std::cout << a.act.act.count << " " << a.act.act.direction << " " << a.act.act.index << std::endl;
		std::cout << testcaseManager::printState( std::bitset<128U>(std::bitset<64>(a.state._2).to_string() + std::bitset<64>(a.state._1).to_string())) << std::endl;
	}
	std::cout << "NOT IN: " << std::endl;
	for (auto a : listNotIn) {
		std::cout << a.count << " " << a.direction << " " << a.index << std::endl;
	}
	std::cout << std::endl;
}

//int main() {
//	std::bitset<128U> bitsetState{ "00000001010010100101011010100001010010100000000000000000000000000000000000000000000000000000001010000101001010100101011010000101" };
//
//	logic::bitState bs;
//	bs._2 = 0b0000000101001010010101101010000101001010000000000000000000000000;
//	bs._1 = 0b0000000000000000000000000000001010000101001010100101011010000101;
//	/*for (int i = 1; i <= 16; ++i) {
//		std::string path = "../Test/Test" + std::to_string(i) + ".input";
//		bool isBlackTurn = true;
//		auto state = testcaseManager::notationToState(path, &isBlackTurn);
//		std::cout << testcaseManager::printState(state[0]) << std::endl;
//		test(state[0], isBlackTurn);
//	}*/
//	test(bs, bitsetState, false);
//
//	int j;
//	std::cin >> j;
//	return 0;
//}


//int main() {
//	std::bitset<128U> bitsetState{ "00000001010010100101011010100001010010100000000000000000000000000000000000000000000000000000001010000101001010100101011010000101" };
//
//	logic::bitState bs;
//	bs._2 = 0b0000000101001010010101101010000101001010000000000000000000000000;
//	bs._1 = 0b0000000000000000000000000000001010000101001010100101011010000101;
//	sf::Clock clock;
//	
//	int fk = 1000;
//	clock.restart();
//	for (int i = 0; i < fk; ++i) {
//		auto dsa = logic::getAllValidMoveOrdered(bitsetState, false);
//	}
//	std::cout << clock.getElapsedTime().asMicroseconds() << std::endl;
//
//	clock.restart();
//	for (int i = 0; i < fk; ++i) {
//		auto dsa = logic::getAllValidMove(bitsetState, false);
//	}
//	std::cout << clock.getElapsedTime().asMicroseconds() << std::endl;
//
//	clock.restart();
//	for (int i = 0; i < fk; ++i) {
//		auto dsa = logic::getAllValidMoveOrdered(bs, false);
//	}
//	std::cout << clock.getElapsedTime().asMicroseconds() << std::endl;
//	auto asdasdsad = logic::b2b(bitsetState);
//	std::cout << logic::b2b(asdasdsad).to_string() << std::endl;
//
//	int j;
//	std::cin >> j;
//	return 0;
//}

#pragma once

#include "gui.h"

int main() {
	//gui g{};
	//g.start();

	using namespace std;

	/*bitset<128U> s{ game::STANDARD_STR };
	auto v = logic::getAllValidMove(s, 1);

	cout << v.size() << endl;
	for (auto r : v) {
		cout << r.first.count << " " << r.first.index << " " << r.first.direction << endl;
	}*/
	bool isBlackTurn = true;
	auto input = logic::notationToState("../Test/Test2.input", &isBlackTurn);
	auto generated = logic::getAllValidMove(input[0], isBlackTurn);
	auto output = logic::notationToState("../Test/Test2.board");
	cout << generated.size() << "/" << output.size() << endl;
	int matched = 0;
	for (auto g : generated) {
		for (auto o : output) {
			if (o == g.second)
				++matched;
		}
	}
	cout << matched << "/" << output.size() << " matched" << endl;
	return 0;
}

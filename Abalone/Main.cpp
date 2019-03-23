#pragma once

#include "gui.h"

void runTestcase(int num) {
	using namespace std;

	cout << "Test Case " << num << endl;
	string path = "../Test/Test" + to_string(num) + ".";
	bool isBlackTurn = true;
	auto input = logic::notationToState(path + "input", &isBlackTurn);
	cout << logic::printState(input[0]);
	auto generated = logic::getAllValidMove(input[0], isBlackTurn);
	auto output = logic::notationToState(path + "board");
	auto outputSize = output.size();
	cout << generated.size() << "/" << outputSize << endl;
	int matched = 0;
	for (auto iter = generated.begin(); iter != generated.end();) {
		auto iter2 = find(output.begin(), output.end(), iter->second);
		if (iter2 == output.end()) {
			++iter;
		}
		else {
			++matched;
			iter = generated.erase(iter);
			output.erase(iter2);
		}
	}
	cout << matched << "/" << outputSize << " matched" << endl;

	if (generated.size() != 0)
		cout << endl << "Exceeded:" << endl;
	for (auto v : generated) {
		cout << endl << logic::printState(v.second) << endl;
		cout << v.first.count << " " << v.first.index << " " << v.first.direction << endl;
	}
	if (output.size() != 0)
		cout << endl << "Don't have:" << endl;
	for (auto v : output) {
		cout << endl << logic::printState(v) << endl;
	}
	cout << endl;
}

int main() {
	gui g{};
	g.start();

	/*using namespace std;

	bitset<128U> s{ game::STANDARD_STR };
	auto v = logic::getAllValidMove(s, 1);

	cout << v.size() << endl;
	for (auto r : v) {
		cout << r.first.count << " " << r.first.index << " " << r.first.direction << endl;
	}
	for (int i = 1; i <= 12; ++i) {
		func(i);
	}

	int dummy;
	cin >> dummy;*/
	return 0;
}



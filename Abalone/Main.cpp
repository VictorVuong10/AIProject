#pragma once

#include "gui.h"
#include "testcaseManager.h"


int main() {

	//testcaseManager::runAllTestcases();
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



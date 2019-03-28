#pragma once

#include <vector>
#include <fstream>
#include <sstream>
#include <bitset>
#include "logic.h"

class testcaseManager
{
public:
	static constexpr unsigned int TESTCASE_NUMBER = 16;

	~testcaseManager() = default;

	testcaseManager(const testcaseManager &) = delete;
	void operator=(const testcaseManager &) = delete;

	static std::vector<std::bitset<128U>> notationToState(const std::string & path, bool * readTurn = 0);
	static std::string stateToNotation(std::bitset<128U> state);
	static void writeOutputToFile(const std::string & path, std::vector<std::pair<logic::action, std::bitset<128U>>> states);
	static std::string printState(std::bitset<128U> state);

	static void runAllTestcases();

	static void runTestcase(int num);
	
private:
	testcaseManager() = default;

	static int rowColToIndex(char row, int col);
};


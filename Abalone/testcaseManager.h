#pragma once

#include <vector>
#include <set>
#include <fstream>
#include <sstream>
#include <bitset>
#include "logic.h"

class testcaseManager
{
public:
	static constexpr unsigned int TESTCASE_NUMBER = 1000;

	~testcaseManager() = default;

	testcaseManager(const testcaseManager &) = delete;
	void operator=(const testcaseManager &) = delete;

	static std::vector<std::bitset<128U>> notationToState(const std::string & path, bool * readTurn = 0);
	static std::string stateToNotation(std::bitset<128U> state);
	static void writeOutputToFile(const std::string & path, std::vector<std::pair<logic::action, std::bitset<128U>>> states);
	static std::string printState(std::bitset<128U> state);

	static void runAllTestcases(int testcaseNum);

	//static void runAllTestcases();

	static void runTestcase(int num);
	static void runAllTestcasesCompare(int testcaseNum);
	static void runTestcaseCompare(int num);
	static void compareWithBoardSet(std::multiset<logic::weightedActionState_old, std::greater<logic::weightedActionState_old>> generated, std::string path);
	static void compareWithBoard(std::vector<std::pair<logic::action, std::bitset<128U>>> generated, std::string path);
	static void runAllTestcasesCompareNew(int testcaseNum);
	static void runTestcaseCompareNew(int num);
	static void compareWithBoardSet(std::multiset<logic::weightedActionState, std::greater<logic::weightedActionState>> generated, std::string path);
	
private:
	testcaseManager() = default;

	static int rowColToIndex(char row, int col);
};


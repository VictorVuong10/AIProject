#include "testcaseManager.h"

std::vector<std::bitset<128U>> testcaseManager::notationToState(const std::string& path, bool* readTurn) {
	std::ifstream file;
	file.open(path);
	if (file.fail()) {
		std::cout << "FAILED TO OPEN " << path << std::endl;
		return {};
	}
	std::vector<std::bitset<128U>> validStates;
	std::string line;
	if (readTurn) {
		std::getline(file, line);
		char turn;
		std::istringstream iss{ line };
		iss >> turn;
		*readTurn = (turn == 'b');
	}
	while (std::getline(file, line)) {
		std::string token;
		std::bitset<128U> state;
		std::istringstream ols{ line };
		while (std::getline(ols, token, ',')) {
			std::istringstream ls{ token };
			char row, color;
			int col;
			ls >> row >> col >> color;
			int index = rowColToIndex(row, col) << 1;
			if (color == 'b') ++index;
			state.set(index);
		}
		validStates.push_back(state);
	}
	return validStates;
}

int testcaseManager::rowColToIndex(char row, int col) {
	auto rowI = 8u - row + 'A';
	int index = 0;
	for (auto i = 0u; i < rowI; ++i)
		index += board::ROW_NUMBER_MAP[i];
	--col;
	return index + (rowI < 4 ? (col - 9 + board::ROW_NUMBER_MAP[rowI]) : col);
}

std::string testcaseManager::stateToNotation(std::bitset<128U> state) {
	std::string res;
	auto cmp = [](std::string a, std::string b) { 
		return a[2] == b[2] ?
					a[0] == b[0] ?
						a[1] < b[1] : a[0] < b[0]
					: a[2] < b[2]; 
	};
	std::set<std::string, decltype(cmp)> sorter{cmp};
	auto stateIndex = 0u;
	for (auto row = 0u; row < 9; ++row) {
		char rowChar = 8u - row + 'A';
		auto colNum = board::ROW_NUMBER_MAP[row];
		for (auto col = 0u; col < colNum; ++col) {
			bool white = state[stateIndex];
			if (white || state[stateIndex + 1]) {
				auto rCol = row < 4 ? 10 - colNum + col : col + 1;
				sorter.insert(rowChar + std::to_string(rCol) + (white ? "w" : "b"));
			}
			stateIndex += 2;
		}
	}
	for (auto s : sorter) {
		res += s;
		res += ',';
	}
	return res.substr(0, res.size() - 1);
}

void testcaseManager::writeOutputToFile(const std::string& path, std::vector<std::pair<logic::action, std::bitset<128U>>> states) {
	std::ofstream stateFile;
	std::ofstream actionFile;
	stateFile.open(path + "board");
	actionFile.open(path + "move");
	if (stateFile.fail() || actionFile.fail()) {
		std::cout << "FAILED TO OPEN " << path << std::endl;
		return;
	}
	for (auto i = 0u; i < states.size(); ++i) {
		auto stateStr = stateToNotation(states[i].second);
		auto action = states[i].first;
		stateFile << stateStr << std::endl;
		actionFile << "count: " << action.count << " index: "
			<< action.index << " direction: " << action.direction << std::endl;
	}
}

std::string testcaseManager::printState(std::bitset<128U> state) {
	std::string res;
	auto slotIndex = 0u;
	for (int i = 0; i < 9; ++i) {
		unsigned int slotNumber = board::ROW_NUMBER_MAP[i];
		auto space = 9u - slotNumber;
		for (auto ss = 0u; ss < space; ++ss) {
			res += ' ';
		}
		for (size_t j = 0; j < slotNumber; ++j) {
			auto slotState = state[slotIndex] << 1 | state[slotIndex + 1] << 0;

			if (slotState == 0) {
				res += '+';
			}
			else if (slotState == 1) {
				res += '@';
			}
			else {
				res += 'O';
			}
			slotIndex += 2;
			res += ' ';
		}
		res += '\n';
	}
	return res;
}

void testcaseManager::runAllTestcases(int testcaseNum) {
	for (int i = 1; i <= testcaseNum; ++i) {
		runTestcase(i);
	}
}

void testcaseManager::runTestcase(int num) {
	using namespace std;

	cout << "Test Case " << num << endl;
	string path = "../Test/Test" + to_string(num) + ".";
	bool isBlackTurn = true;
	auto input = notationToState(path + "input", &isBlackTurn);
	cout << printState(input[0]);
	auto generated = logic::getAllValidMove(input[0], isBlackTurn);
	writeOutputToFile(path, generated);
	
	cout << endl;
}

void testcaseManager::runAllTestcasesCompare(int testcaseNum) {
	for (int i = 1; i <= testcaseNum; ++i) {
		runTestcaseCompare(i);
	}
}

void testcaseManager::runTestcaseCompare(int num) {
	using namespace std;

	cout << "Test Case " << num << endl;
	string path = "../Test/Test" + to_string(num) + ".";
	bool isBlackTurn = true;
	auto input = notationToState(path + "input", &isBlackTurn);
	cout << printState(input[0]);
	auto generated = logic::getAllValidMoveOrdered(input[0], isBlackTurn);
	compareWithBoardSet(generated, path);

	cout << endl;
}


void testcaseManager::compareWithBoardSet(std::multiset<logic::weightedActionState, std::greater<logic::weightedActionState>> generated, std::string path) {

	auto output = notationToState(path + "board");
	auto outputSize = output.size();
	std::cout << generated.size() << "/" << outputSize << std::endl;
	int matched = 0;
	for (auto iter = generated.begin(); iter != generated.end();) {
		auto temp = iter->state;
		temp <<= 6;
		temp >>= 6;
		auto iter2 = find(output.begin(), output.end(), temp);
		if (iter2 == output.end()) {
			++iter;
		}
		else {
			++matched;
			iter = generated.erase(iter);
			output.erase(iter2);
		}
	}
	std::cout << matched << "/" << outputSize << " matched" << std::endl;

	if (generated.size() != 0)
		std::cout << std::endl << "Exceeded:" << std::endl;
	for (auto v : generated) {
		std::cout << std::endl << printState(v.state) << std::endl;
		std::cout << v.act.act.count << " " << v.act.act.index << " " << v.act.act.direction << std::endl;
	}
	if (output.size() != 0)
		std::cout << std::endl << "Don't have:" << std::endl;
	for (auto v : output) {
		std::cout << std::endl << printState(v) << std::endl;
	}
}

void testcaseManager::compareWithBoard(std::vector<std::pair<logic::action, std::bitset<128U>>> generated, std::string path)
{
	auto output = notationToState(path + "board");
	auto outputSize = output.size();
	std::cout << generated.size() << "/" << outputSize << std::endl;
	int matched = 0;
	for (auto iter = generated.begin(); iter != generated.end();) {
		auto temp = iter->second;
		temp <<= 6;
		temp >>= 6;
		iter->second = temp;
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
	std::cout << matched << "/" << outputSize << " matched" << std::endl;

	if (generated.size() != 0)
		std::cout << std::endl << "Exceeded:" << std::endl;
	for (auto v : generated) {
		std::cout << std::endl << printState(v.second) << std::endl;
		std::cout << v.first.count << " " << v.first.index << " " << v.first.direction << std::endl;
	}
	if (output.size() != 0)
		std::cout << std::endl << "Don't have:" << std::endl;
	for (auto v : output) {
		std::cout << std::endl << printState(v) << std::endl;
	}
}

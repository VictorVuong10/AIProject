#include "game.h"
#include "gui.h"

/*
		1010101010
	   101010101010
	  00001010100000
	 0000000000000000
	000000000000000000
	 0000000000000000
	  00000101010000
	   010101010101
		0101010101
*/

const std::string game::STANDARD_STR = "10101010101010101010100000101010000000000000000000000000000000000000000000000000000000000001010100000101010101010101010101";

/*
		0000000000
	   101000000101
	  10101000010101
	 0010100000010100
	000000000000000000
	 0001010000101000
	  01010100101010
	   010100001010
		0000000000
*/
const std::string game::GERMAN_DAISY_STR = "00000000001010000001011010100001010100101000000101000000000000000000000001010000101000010101001010100101000010100000000000";

/*
		1010000101
	   101010010101
	  00101000010100
	 0000000000000000
	000000000000000000
	 0000000000000000
	  00010100101000
	   010101101010
		0101001010
*/
const std::string game::BELGAIN_DAISY_STR = "10100001011010100101010010100001010000000000000000000000000000000000000000000000000000000101001010000101011010100101001010";


const std::string game::INIT_STATES[4] = { "", STANDARD_STR, GERMAN_DAISY_STR, BELGAIN_DAISY_STR };


game::game() : player1IsHuman{ true }, player2IsHuman{ false }, state{ INIT_STATES[EMPTY] },
	progress{ gameProgress::NOT_STARTED }, rman{ &resourceManager::instance }, storedSec{ 0 }, isBlackTurn{ true }
{
	initAllEle();
}

game::game(gui* ui) : player1IsHuman{ true }, player2IsHuman{ false }, state{ INIT_STATES[EMPTY] },
	ui{ ui }, progress{ gameProgress::NOT_STARTED }, rman{ &resourceManager::instance }, storedSec{ 0 }, isBlackTurn{true}
{
	initAllEle();
}

game::~game()
{
	delete gameBoard;
	delete startBtn;
	delete pauseBtn;
	delete undoBtn;
	delete resetBtn;
	for (auto b : moveBtn) {
		delete b;
	}
}

bool game::checkClick(sf::Event &)
{
	return false;
}

void game::click(sf::Event & e)
{
	if (progress == gameProgress::IN_PROGRESS) {
		gameBoard->click(e);
		for (auto b : moveBtn) {
			b->click(e);
		}
	}
	startBtn->click(e);
	pauseBtn->click(e);
	undoBtn->click(e);
	resetBtn->click(e);
	standardBtn->click(e);
	germanDaisyBtn->click(e);
	belgainDaisyBtn->click(e);
	player1ChangeBtn->click(e);
	player2ChangeBtn->click(e);
}

void game::show(sf::RenderWindow & window)
{
	startBtn->show(window);
	gameBoard->show(window);
	pauseBtn->show(window);
	undoBtn->show(window);
	resetBtn->show(window);
	standardBtn->show(window);
	germanDaisyBtn->show(window);
	belgainDaisyBtn->show(window);
	player1ChangeBtn->show(window);
	player2ChangeBtn->show(window);
	window.draw(boardSetupLabel);
	window.draw(playerChangeLabel);
	window.draw(blackLostText);
	window.draw(whiteLostText);
	setTimer();
	window.draw(timerText);
	for (auto b : moveBtn) {
		b->show(window);
	}
	/*window.draw(log);*/
}

void game::initAllEle()
{
	initBoard();
	initTimer();
	initStartBtn();
	initPauseBtn();
	initUndoBtn();
	initResetBtn();
	initboardSetupBtn();
	initPlayerChangeBtn();
	initMoveBtn();
	initScore();
	initLog();
}

void game::initBoard()
{
	gameBoard = new board{ 600, 400, 300, this, state };
}

void game::initTimer()
{
	sf::Font &arial = rman->getFont("arial");
	timerText.setFont(arial);
	timerText.setString("Timer: 0:0");
	timerText.setFillColor(sf::Color::Red);
	timerText.setCharacterSize(50);
	timerText.setPosition(450, 0);
}

void game::initStartBtn()
{
	startBtn = new button{ 100, "start", {50, 100}, sf::Color::Red };
	auto handler = new std::function<void(sf::Event&)>
	{ 
		[&](sf::Event& e) 
		{
			startGame();
		}
	};
	startBtn->registerHandler(handler);
}

void game::startGame() {
	if (state == std::bitset<128U>{INIT_STATES[EMPTY]}) {
		return;
	}
	if (gameProgress::NOT_STARTED == progress) {
		gameState toBeSaved = { player1IsHuman, player2IsHuman, state, storedSec, isBlackTurn };
		history.push(toBeSaved);
	}
	progress = gameProgress::IN_PROGRESS;
	clock.restart();
}

void game::initPauseBtn()
{
	pauseBtn = new button{ 100, "pause", {50, 160}, sf::Color::Red };
	auto handler = new std::function<void(sf::Event&)>
	{ 
		[&, this](sf::Event& e) 
		{
			if (progress != gameProgress::IN_PROGRESS)
				return;
			this->progress = gameProgress::PAUSED;
			this->storedSec += this->clock.getElapsedTime().asSeconds();
		}
	};
	pauseBtn->registerHandler(handler);
}

void game::initUndoBtn()
{
	undoBtn = new button{ 100, "undo", {50, 220}, sf::Color::Red };
	auto handler = new std::function<void(sf::Event&)>{ [&](sf::Event& e) {
			if (progress != gameProgress::IN_PROGRESS || history.size() < 2)
				return;
			history.pop();
			gameState lastState = history.top();
			state = lastState.state;
			gameBoard->setState(state);
			storedSec = lastState.storedSec;
			isBlackTurn = lastState.isBlackTurn;
			clock.restart();
		}
	};
	undoBtn->registerHandler(handler);
}

void game::initResetBtn() {
	resetBtn = new button{ 100, "reset", {50, 280}, sf::Color::Red };
	auto handler = new std::function<void(sf::Event&)>{ [&](sf::Event& e) {
			if (progress == gameProgress::NOT_STARTED)
				return;
			progress = gameProgress::NOT_STARTED;
			history = {};
			state = std::bitset<128U>{ INIT_STATES[EMPTY] };
			gameBoard->setState(state);
			player1IsHuman = true;
			player2IsHuman = false;
			selectedIndex = {};
			storedSec = 0;
			isBlackTurn = true;
			timerText.setString("Timer: 0:0");
			blackLostText.setString("Black Lost: 0");
			whiteLostText.setString("White Lost: 0");
		}
	};
	resetBtn->registerHandler(handler);
}


void game::initboardSetupBtn() {
	sf::Font &arial = rman->getFont("arial");
	boardSetupLabel.setFont(arial);
	boardSetupLabel.setString("Choose board initial setup: ");
	boardSetupLabel.setFillColor(sf::Color::Red);
	boardSetupLabel.setCharacterSize(25);
	boardSetupLabel.setPosition(850, 50);
	standardBtn = new button{ 70, "Standard", {1000, 100}, sf::Color::Green };
	germanDaisyBtn = new button{ 70, "German Daisy", {1000, 140}, sf::Color::Green };
	belgainDaisyBtn = new button{ 70, "Belgain Daisy", {1000, 180}, sf::Color::Green };
	standardBtn->getBackground().setSize({ 150, 35 });
	germanDaisyBtn->getBackground().setSize({ 150, 35 });
	belgainDaisyBtn->getBackground().setSize({ 150, 35 });
	auto standardHandler = new std::function<void(sf::Event&)>{ [&](sf::Event& e) {
			if (progress != gameProgress::NOT_STARTED)
				return;
			state = std::bitset<128U>{ INIT_STATES[STANDARD] };
			gameBoard->setState(state);
		}
	};
	standardBtn->registerHandler(standardHandler);
	auto germanDaisyHandler = new std::function<void(sf::Event&)>{ [&](sf::Event& e) {
			if (progress != gameProgress::NOT_STARTED)
				return;
			state = std::bitset<128U>{ INIT_STATES[GERMAN_DAISY] };
			gameBoard->setState(state);
		}
	};
	germanDaisyBtn->registerHandler(germanDaisyHandler);
	auto belgainDaisyHandler = new std::function<void(sf::Event&)>{ [&](sf::Event& e) {
			if (progress != gameProgress::NOT_STARTED)
				return;
			state = std::bitset<128U>{ INIT_STATES[BELGAIN_DAISY] };
			gameBoard->setState(state);
		}
	};
	belgainDaisyBtn->registerHandler(belgainDaisyHandler);
}

void game::initPlayerChangeBtn() {
	sf::Font &arial = rman->getFont("arial");
	playerChangeLabel.setFont(arial);
	playerChangeLabel.setString("Change player: ");
	playerChangeLabel.setFillColor(sf::Color::Red);
	playerChangeLabel.setCharacterSize(25);
	playerChangeLabel.setPosition(950, 250);
	player1ChangeBtn = new button{ 70, "Player1: Human", {1000, 300}, sf::Color::Green };
	player2ChangeBtn = new button{ 70, "Player1: AI", {1000, 340}, sf::Color::Green };
	player1ChangeBtn->getBackground().setSize({ 160, 35 });
	player2ChangeBtn->getBackground().setSize({ 160, 35 });
	auto player1ChangeHandler = new std::function<void(sf::Event&)>{ [&](sf::Event& e) {
			if (progress != gameProgress::NOT_STARTED)
				return;
			player1IsHuman = !player1IsHuman;
			std::string str = player1IsHuman ? "Human" : "AI";
			player1ChangeBtn->getText()->setString("Player1: " + str);
		}
	};
	player1ChangeBtn->registerHandler(player1ChangeHandler);
	auto player2ChangeHandler = new std::function<void(sf::Event&)>{ [&](sf::Event& e) {
			if (progress != gameProgress::NOT_STARTED)
				return;
			player2IsHuman = !player2IsHuman;
			std::string str = player2IsHuman ? "Human" : "AI";
			player2ChangeBtn->getText()->setString("Player2: " + str);
		}
	};
	player2ChangeBtn->registerHandler(player2ChangeHandler);
}

void game::initScore()
{
	sf::Font &arial = rman->getFont("arial");
	blackLostText.setFont(arial);
	blackLostText.setString("Black Lost: 0");
	blackLostText.setFillColor(sf::Color::Red);
	blackLostText.setCharacterSize(24);
	blackLostText.setPosition(520, 690);
	whiteLostText.setFont(arial);
	whiteLostText.setString("White Lost: 0");
	whiteLostText.setFillColor(sf::Color::Red);
	whiteLostText.setCharacterSize(24);
	whiteLostText.setPosition(520, 90);
}

void game::initMoveBtn() {
	moveBtn[0] = new button{ 100, "<-", {40, 650}, sf::Color::Blue };
	moveBtn[1] = new button{ 100, "UL", {70, 580}, sf::Color::Blue };
	moveBtn[2] = new button{ 100, "UR", {150, 580}, sf::Color::Blue };
	moveBtn[3] = new button{ 100, "->", {180, 650}, sf::Color::Blue };
	moveBtn[4] = new button{ 100, "DR", {150, 720}, sf::Color::Blue };
	moveBtn[5] = new button{ 100, "DL", {70, 720}, sf::Color::Blue };
	moveBtn[0]->getText()->setString(L"←");
	moveBtn[1]->getText()->setString(L"↑");
	auto f21 = moveBtn[1]->getText()->getPosition();
	moveBtn[1]->getText()->setPosition({ f21.x, f21.y + 8 });
	moveBtn[1]->getText()->rotate(-45.f);
	moveBtn[2]->getText()->setString(L"→");
	auto f22 = moveBtn[2]->getText()->getPosition();
	moveBtn[2]->getText()->setPosition({ f22.x - 3, f22.y + 14 });
	moveBtn[2]->getText()->rotate(-45.f);
	moveBtn[3]->getText()->setString(L"→");
	moveBtn[4]->getText()->setString(L"↓");
	auto f24 = moveBtn[4]->getText()->getPosition();
	moveBtn[4]->getText()->setPosition({ f24.x, f24.y + 8 });
	moveBtn[4]->getText()->rotate(-45.f);
	moveBtn[5]->getText()->setString(L"←");
	auto f25 = moveBtn[5]->getText()->getPosition();
	moveBtn[5]->getText()->setPosition({ f25.x - 3, f25.y + 14 });
	moveBtn[5]->getText()->rotate(-45.f);
	for (int i = 0; i < 6; ++i) {
		moveBtn[i]->getBackground().setSize({ 60, 50 });
		auto handler = new std::function<void(sf::Event&)>{ [&, i](sf::Event& e) {
			ui->asyncAwait<std::bitset<128U>>([&, i] {
				
				std::bitset<128U> nstate{ game::STANDARD_STR };
				int minIndex = 62;
				for (auto index : selectedIndex) {
					if (index < minIndex) {
						minIndex = index;
					}
				}
				std::cout << "selected number: " << selectedIndex.size()
					<< " Index: " << minIndex << " direction: " << i << std::endl;

				std::this_thread::sleep_for(std::chrono::seconds{ 2 });
				return nstate << (i * 2);
			}, [&](std::bitset<128U> state) {
				if (gameProgress::IN_PROGRESS != progress)
					return;
				nextState(state);
			});			
		} };
		moveBtn[i]->registerHandler(handler);
	}
}

void game::initLog()
{
}

void game::setTimer() {
	if (progress == gameProgress::IN_PROGRESS) {
		auto time = clock.getElapsedTime();
		auto second = time.asSeconds();
		float tempSec = storedSec + second;
		int min = static_cast<int>(tempSec) / 60;
		int sec = static_cast<int>(tempSec) % 60;
		timerText.setString("Timer: " + std::to_string(min) + ":" + std::to_string(sec));
	}
}

bool isBlackPiece(int index, std::bitset<128U> state) {
		return (state[(index * 2) + 0] == 0 && state[(index * 2) + 1] == 1);
}

bool isWhitePiece(int index, std::bitset<128U> state) {
	return (state[(index * 2) + 0] == 1 && state[(index * 2) + 1] == 0);
}

bool isEmpty(int index, std::bitset<128U> state) {
	return (state[(index * 2) + 0] == 0 && state[(index * 2) + 1] == 0);
}

std::bitset<128U> game::moveHelper(int marbleCount, int index, int direction, int checkDirection, int secondCheckDirection) {

	int storedIndex = index;

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

		if (!isEmpty) {
			//i think 01 = black
			std::cout << "yes2.0";


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
			}

			//didnt calculate white scenario yet.

			else {
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
			newState.set((storedIndex * 2) + 0, 0);
			newState.set((storedIndex * 2) + 1, 0);
			newState.set(((MOVE_TABLE[storedIndex][direction]) * 2) + 0, 0);
			newState.set(((MOVE_TABLE[storedIndex][direction]) * 2) + 1, 1);

			//just so index does not become -1 but i dont think this matters.
			if (i < marbleCount - 1)
				storedIndex = MOVE_TABLE[storedIndex][checkDirection];
		}
	}
	else {
		for (int i = 0; i < marbleCount; i++) {
			newState.set((storedIndex * 2) + 0, 0);
			newState.set((storedIndex * 2) + 1, 0);
			newState.set(((MOVE_TABLE[storedIndex][direction]) * 2) + 0, 1);
			newState.set(((MOVE_TABLE[storedIndex][direction]) * 2) + 1, 0);

			if (i < marbleCount - 1)
				storedIndex = MOVE_TABLE[storedIndex][checkDirection];

		}
	}


	return newState;


}






std::bitset<128U> game::move(int marbleCount, int index, int direction) {

	std::cout << marbleCount << " asdasd : " << index << "asdasd \n";

	if (marbleCount == 1) {
		
		int initialIndex = index;

		if (isBlackTurn) {
			
			int blackMarbleCount = 0;

			//index is the index to check

			while (isBlackPiece(index, state)) {
				blackMarbleCount++;
				index = MOVE_TABLE[index][direction];
			}

			if (blackMarbleCount > 3) {
				return state;
			}

			if (isEmpty(index, state)) {
				std::bitset<128U> newState = state;

				newState.set((initialIndex * 2) + 0, 0);
				newState.set((initialIndex * 2) + 1, 0);
				newState.set((index * 2) + 0, 0);
				newState.set((index * 2) + 1, 1);

				return newState;
			}

			//stil have to do the part where the marbles push each other.


		}
		else {
			//white scenario

			int whiteMarbleCount = 0;

			//index is the index to check

			while (isBlackPiece(index, state)) {
				whiteMarbleCount++;
				index = MOVE_TABLE[index][direction];
			}

			if (whiteMarbleCount > 3) {
				return state;
			}

			if (isEmpty(index, state)) {
				std::bitset<128U> newState = state;

				newState.set((initialIndex * 2) + 0, 0);
				newState.set((initialIndex * 2) + 1, 0);
				newState.set((index * 2) + 0, 1);
				newState.set((index * 2) + 1, 0);

				return newState;
			}

			//pushing case here

		}

	}

	else {

		if (direction == 0) {

			//4 is the check direction, 5 is the second possible check direction
			return moveHelper(marbleCount, index, direction, 4, 5);

		} else if (direction == 1) {

			//4 is the check direction, 5 is the second possible check direction
			return moveHelper(marbleCount, index, direction, 5, 1);

		} else if (direction == 2) {

			//4 is the check direction, 5 is the second possible check direction
			return moveHelper(marbleCount, index, direction, 4, 3);

		} else if (direction == 3) {

			//4 is the check direction, 5 is the second possible check direction
			return moveHelper(marbleCount, index, direction, 5, 4);

		} else if (direction == 4) {

			//4 is the check direction, 5 is the second possible check direction
			return moveHelper(marbleCount, index, direction, 2, 3);

		}
		//direction == 5 i probably shoulda done a switch statement but screw it.
		return moveHelper(marbleCount, index, direction, 3, 4);

	}

	
}

void game::nextState(std::bitset<128U> state) {
	gameBoard->setState(state);
	gameState toBeSaved = { player1IsHuman, player2IsHuman, state, storedSec + clock.getElapsedTime().asSeconds(), isBlackTurn };
	history.push(toBeSaved);
}

bool game::getIsBlackTurn() {
	return isBlackTurn;
}

bool game::trySelect(int index) {
	selectedIndex.push_back(index);
	return true;
}

void game::unSelect(int index) {
	selectedIndex.erase(std::find(selectedIndex.begin(), selectedIndex.end(), index));
}
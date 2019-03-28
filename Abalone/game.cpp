#include "game.h"
#include "gui.h"

//std::string is readed backward into std::bitset

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

const std::string game::STANDARD_STR = "00000010101010101010101010100000101010000000000000000000000000000000000000000000000000000000000001010100000101010101010101010101";

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
const std::string game::GERMAN_DAISY_STR = "00000000000000001010000001011010100001010100101000000101000000000000000000000001010000101000010101001010100101000010100000000000";

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
const std::string game::BELGAIN_DAISY_STR = "00000010100001011010100101010010100001010000000000000000000000000000000000000000000000000000000101001010000101011010100101001010";


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
	delete stopBtn;
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
	stopBtn->click(e);
	pauseBtn->click(e);
	undoBtn->click(e);
	resetBtn->click(e);
	standardBtn->click(e);
	germanDaisyBtn->click(e);
	belgainDaisyBtn->click(e);
	player1ChangeBtn->click(e);
	player2ChangeBtn->click(e);
	p1IsBlackBtn->click(e);

}

void game::show(sf::RenderWindow & window)
{
	startBtn->show(window);
	stopBtn->show(window);
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
	window.draw(maxMovesBox);
	window.draw(maxMovesTitle);
	window.draw(maxMovesEditText);
	window.draw(moveTimeLimitBox);
	window.draw(moveTimeLimitTitle);
	window.draw(moveTimeLimitEditText);
	window.draw(moveTimeLimitBox2);
	window.draw(moveTimeLimitTitle2);
	window.draw(moveTimeLimitEditText2);
	window.draw(nextMoveText);
	window.draw(timerTextBlack);
	window.draw(timerTextWhite);
	window.draw(p1IsBlackText);
	window.draw(moveTimerWhite);
	window.draw(moveTimerBlack);
	p1IsBlackBtn->show(window);
	setTimer();
	setTurnTimer();
	setMoveTimer();
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
	initStopBtn();
	initPauseBtn();
	initUndoBtn();
	initResetBtn();
	initboardSetupBtn();
	initPlayerChangeBtn();
	initMoveBtn();
	initScore();
	initLog();
	initMaxMoves();
	initMoveTimeLimit();
	initPlayerColorBtns();
	initBlackTimer();
	initWhiteTimer();
	initMoveTimer();
	initNextMove();
}

void game::initBoard()
{
	gameBoard = new board{ 600, 400, 300, this, state };
}

void game::initTimer()
{
	setUpText(timerText, "Timer: 0:0", 450, 0);
	timerText.setCharacterSize(50);
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
		gameState toBeSaved = { state, storedSec, isBlackTurn };
		history.push(toBeSaved);
	}
	progress = gameProgress::IN_PROGRESS;
	clock.restart();
	turnTimer.restart();
	std::cout << "Game START." << std::endl << std::endl;
}

void game::initPauseBtn()
{
	//need turnTimer store
	pauseBtn = new button{ 100, "pause", {50, 160}, sf::Color::Red };
	auto handler = new std::function<void(sf::Event&)>
	{ 
		[&, this](sf::Event& e) 
		{
			if (progress != gameProgress::IN_PROGRESS)
				return;
			this->progress = gameProgress::PAUSED;
			this->storedSec += this->clock.getElapsedTime().asSeconds();
			std::cout << "storedTurnSec" << turnTimer.getElapsedTime().asSeconds();
			this->storedTurnSec += turnTimer.getElapsedTime().asSeconds();
			std::cout << "Game is paused." << std::endl << std::endl;
		}
	};
	pauseBtn->registerHandler(handler);
}

void game::initStopBtn()
{
	stopBtn = new button{ 100, "stop", {50, 220}, sf::Color::Red };
	auto handler = new std::function<void(sf::Event&)>
	{
		[&, this](sf::Event& e)
		{
			if (progress != gameProgress::IN_PROGRESS)
				return;
			this->progress = gameProgress::PAUSED;
			this->storedSec += this->clock.getElapsedTime().asSeconds();
			this->storedTurnSec += turnTimer.getElapsedTime().asSeconds();
			std::cout << "Total time elapsed: " << clock.getElapsedTime().asSeconds() << std::endl;
			std::cout << "Game has stopped. " << std::endl << std::endl;
			stopGame();
		}
	};

	stopBtn->registerHandler(handler);
}

void game::initUndoBtn()
{
	//need to update for my state variables
	undoBtn = new button{ 100, "undo", {50, 280}, sf::Color::Red };
	auto handler = new std::function<void(sf::Event&)>{ [&](sf::Event& e) {
			if (progress != gameProgress::IN_PROGRESS || history.size() < 2)
				return;
			history.pop();
			gameState lastState = history.top();
			state = lastState.state;
			gameBoard->setState(state);
			setScoreFromState(state);
			storedSec = lastState.storedSec;
			storedTurnSec = 0;
			isBlackTurn = lastState.isBlackTurn;
			clock.restart();
			turnTimer.restart();
			std::cout << "Undo last move." << std::endl << std::endl;
		}
	};
	undoBtn->registerHandler(handler);
}

void game::initResetBtn() {
	resetBtn = new button{ 100, "reset", {50, 340}, sf::Color::Red };
	auto handler = new std::function<void(sf::Event&)>{ [&](sf::Event& e) {
			if (progress == gameProgress::NOT_STARTED)
				return;
			// Here's the test
			progress = gameProgress::NOT_STARTED;
			history = {};
			state = std::bitset<128U>{ INIT_STATES[EMPTY] };
			gameBoard->setState(state);
			player1IsHuman = true;
			player2IsHuman = false;
			gameBoard->unSelectAll();
			selectedIndex = {};
			storedSec = 0;
			isBlackTurn = true;
			isP1Black = false;
			timerText.setString("Timer: 0:0");
			moveTimeLimitBlack = 0;
			moveTimeLimitWhite = 0;
			moveTimeLimitEditText.setString("");
			moveTimeLimitEditText2.setString("");
			timerTextBlack.setString("P1: 0.0");
			timerTextWhite.setString("P2: 0.0");
			blackLostText.setString("Black Lost: 0");
			whiteLostText.setString("White Lost: 0");
			std::cout << "Game is reseted." << std::endl << std::endl;
		}
	};
	resetBtn->registerHandler(handler);
}


void game::initboardSetupBtn() {
	setUpText(boardSetupLabel, "Choose board initial setup: ", 850, 50);
	standardBtn = new button{ 70, "Standard", {1000, 100}, sf::Color{ 165, 104, 24 } };
	germanDaisyBtn = new button{ 70, "German Daisy", {1000, 140}, sf::Color{ 165, 104, 24 } };
	belgainDaisyBtn = new button{ 70, "Belgain Daisy", {1000, 180}, sf::Color{ 165, 104, 24 } };
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
	setUpText(playerChangeLabel, "Change players: ", 950, 250);
	player1ChangeBtn = new button{ 70, "Player 1: Human", {1000, 300}, sf::Color{ 91, 44, 6 } };
	player2ChangeBtn = new button{ 70, "Player 2: AI", {1000, 340}, sf::Color{ 91, 44, 6 } };
	player1ChangeBtn->getBackground().setSize({ 170, 35 });
	player2ChangeBtn->getBackground().setSize({ 170, 35 });
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
	setUpText(blackLostText, "Black Lost: 0", 520, 680);
	setUpText(whiteLostText, "White Lost: 0", 520, 90);
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
			/*ui->asyncAwait<std::bitset<128U>>([&, i] {
				
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
				
				nextState(state);
			});		*/
			
			if (selectedIndex.size() == 0)
				return;
			bool moveType = isSideMove(i);
			bool valid = moveType ? isSideMoveValid(i) : isInlineValid(i);
			if (valid) {
				auto action = makeAction(i, moveType);
				std::cout << "player " << (isBlackTurn ? "1(Black)" : "2(White)") << "is moving by action: " << std::endl;
				std::cout << "count: " <<action.count << " index: " << action.index << " direction: " << action.direction << std::endl;
				gameBoard->unSelectAll();
				selectedIndex.clear();
				auto tempState = logic::move(state, action, isBlackTurn);
				isBlackTurn = !isBlackTurn;
				nextState(tempState);
				turnTimer.restart();
				++movesMade;
			}
			
		} };
		moveBtn[i]->registerHandler(handler);
	}
}

logic::action game::makeAction(unsigned short direction, bool isSideMove) {
	if (isSideMove) {
		int drrCcw1 = (direction + 5) % 6;
		bool isHighCcw1 = logic::CLOCKWISE_RIGHT[direction][1];
		int iCcw1 = isHighCcw1 ? *(selectedIndex.end() - 1) : *selectedIndex.begin();
		int iCcw1Next = logic::MOVE_TABLE[iCcw1][drrCcw1];
		for (int is : selectedIndex) {
			if(iCcw1Next == is)
				return { static_cast<int>(selectedIndex.size()), iCcw1, direction };
		}
		int drrCcw2 = (direction + 4) % 6;
		bool isHighCcw2 = logic::CLOCKWISE_RIGHT[direction][0];
		int iCcw2 = isHighCcw2 ? *(selectedIndex.end() - 1) : *selectedIndex.begin();
		return { static_cast<int>(selectedIndex.size()), iCcw2, direction };
	}
	else {
		int index = direction < 3 ? *(selectedIndex.end() - 1) : *selectedIndex.begin();
		return { 1, index, direction };
	}
}

//false inline, true sidemove
bool game::isSideMove(unsigned short direction) {
	if (1 == selectedIndex.size())
		return true;
	int smallest = selectedIndex[0];
	for (int i = 0; i < 6; ++i) {
		if (logic::MOVE_TABLE[smallest][i] == selectedIndex[1])
			return !(i == direction || (i + 3) % 6 == direction);
	}
	throw "inlvaid move";
}

bool game::isActionValid(unsigned short direction, bool isSideMove) {
	return isSideMove ? isSideMoveValid(direction) : isInlineValid(direction);
}

bool game::isInlineValid(unsigned short direction) {
	int first = direction < 3 ? *selectedIndex.begin() : *(selectedIndex.end() - 1);
	int next = logic::MOVE_TABLE[first][direction];
	size_t checkCount = 0;
	if (next == -1)
		return false;
	do {
		if (next == -1)
			return true;
		auto nextState = state[next << 1] << 1 | state[(next << 1) + 1] << 0;
		if (!nextState)
			return true;
		if (nextState & 2 ^ isBlackTurn << 1)
			return false;
		next = logic::MOVE_TABLE[next][direction];
		++checkCount;
	} while (checkCount < selectedIndex.size());
	return false;

}

bool game::isSideMoveValid(unsigned short direction) {
	for (auto index : selectedIndex) {
		int next = logic::MOVE_TABLE[index][direction];
		if (next == -1)
			return false;
		if (state[next << 1] | state[(next << 1) + 1]) {
			return false;
		}
	}
	return true;
}


void game::initLog()
{
}

void game::setTimer() {
	if (progress == gameProgress::IN_PROGRESS) {
		//TODO also do my other timers
		auto time = clock.getElapsedTime();
		auto second = time.asSeconds();
		float tempSec = storedSec + second;
		int min = static_cast<int>(tempSec) / 60;
		int sec = static_cast<int>(tempSec) % 60;
		timerText.setString("Timer: " + std::to_string(min) + ":" + std::to_string(sec));
	}
}

void game::nextState(std::bitset<128U> state) {
	this->state = state;
	gameBoard->setState(state);
	setScoreFromState(state);
	gameState toBeSaved = { state, storedSec + clock.getElapsedTime().asSeconds(), isBlackTurn };
	std::cout << "Time used: " << toBeSaved.storedSec - history.top().storedSec << " seconds." << std::endl << std::endl;
	history.push(toBeSaved);
}

void game::setScoreFromState(std::bitset<128U> state) {
	auto white = 0u, black = 0u;
	/*for (auto i = 124u, j = 127u; i > 121; --i, --j) {
		whiteLost <<= 1;
		blackLost <<= 1;
		whiteLost |= state[j] << 0;
		blackLost |= state[i] << 0     ;
	}*/
	for (auto i = isBlackTurn << 0, j = !isBlackTurn << 0; i < 122; i += 2, j += 2) {
		if (state[i]) ++black;
		if (state[j]) ++white;
	}
	blackLostText.setString("Black Lost: " + std::to_string(14 - white));
	whiteLostText.setString("White Lost: " + std::to_string(14 - black));
}

void game::stopGame()
{
	// 1. Check score
	int whiteScore;
	int blackScore;

	// 2. Sum time taken by each player
	double blackTotalTime = 0.0;
	double whiteTotalTime = 0.0;

	for (int i = 0; i < history.size(); i++) {
		gameState thisTurn = history.top();
		if (thisTurn.isBlackTurn) {
			blackTotalTime += thisTurn.storedSec;
		} else {
			whiteTotalTime += thisTurn.storedSec;
		}
		history.pop();
	}

	// 3. Compare time
	std::cout << "Black took " << blackTotalTime << " seconds to move." << std::endl;
	std::cout << "White took " << whiteTotalTime << " seconds to move." << std::endl;
	std::cout << (blackTotalTime < whiteTotalTime ? "Black" : "White") << " wins!" << std::endl;

	// Set base states so game cannot be restarted without reset.
	history = {};
	state = std::bitset<128U>{ INIT_STATES[EMPTY] };
}

bool game::getIsBlackTurn() {
	return isBlackTurn;
}

bool game::trySelect(int index) {
	bool selected = false;

	switch (selectedIndex.size()) {
	case 0: {
		selectedIndex.push_back(index);
		return true;
	}
	case 1: {
		for (int i = 0; i < 6; ++i) {
			if (logic::MOVE_TABLE[index][i] == selectedIndex[0]) {
				selectedIndex.push_back(index);
				selected = true;
				break;
			}
		}
		if (!selected)
			return selected;
		break;
	}
	case 2:
		for (int i = 0; i < 6; ++i) {
			if ((logic::MOVE_TABLE[index][i] == selectedIndex[0] && logic::MOVE_TABLE[selectedIndex[0]][i] == selectedIndex[1])
				|| (logic::MOVE_TABLE[index][i] == selectedIndex[1] && logic::MOVE_TABLE[selectedIndex[1]][i] == selectedIndex[0])) {
				
				selectedIndex.push_back(index);
				selected = true;
				break;
			}
		}
		if (!selected)
			return selected;
		break;
	default:
		return false;
	}
	std::sort(selectedIndex.begin(), selectedIndex.end());
	return true;
}

bool game::tryUnSelect(int index) {
	auto ptr = std::find(selectedIndex.begin(), selectedIndex.end(), index);
	if (ptr == selectedIndex.begin() || ptr == selectedIndex.end() - 1) {
		selectedIndex.erase(ptr);
		std::sort(selectedIndex.begin(), selectedIndex.end());
		return true;
	}
	return false;
}


void game::setUpText(sf::Text& textElement, std::string text, int x, int y)
{
	//Standardizes non editable text elements
	sf::Font &arial = rman->getFont("arial");
	textElement.setFont(arial);	
	textElement.setPosition(x,y);
	textElement.setString(text);
	textElement.setFillColor(sf::Color(250, 0, 0));
	textElement.setCharacterSize(25);
}

void game::setUpTextBox(sf::Text& textElement, sf::RectangleShape& boxElement, sf::Text& titleElement, int x, int y, std::string title)
{
	//Standardizes all textbox elements
	sf::Font &arial = rman->getFont("arial");
	titleElement.setPosition(x, y);
	titleElement.setString(title);
	titleElement.setFillColor(sf::Color(255,0,0));
	titleElement.setFont(arial);

	boxElement.setSize(sf::Vector2f(170,35));
	boxElement.setFillColor(sf::Color(165,104,24));
	boxElement.setPosition(x, y + 40);

	textElement.setPosition(x, y + 40);
	textElement.setFont(arial);
}

void game::setUpTimer(sf::Text& timer, int x, int y, int timeLimit, std::string text)
{
	//Standardizes all timer elements
	sf::Font &arial = rman->getFont("arial");
	timer.setFont(arial);
	timer.setString(text + std::to_string(timeLimit));
	timer.setFillColor(sf::Color::Red);
	timer.setCharacterSize(25);
	timer.setPosition(x, y);
}

void game::initMaxMoves()
{
	setUpTextBox(maxMovesEditText, maxMovesBox, maxMovesTitle, 1000, 380, "Move Limit: ");
}

void game::setMaxMovesEditText(sf::String maxMoves)
{
	if (progress == game::NOT_STARTED)
	{
		std::string temp = maxMovesEditText.getString();
		temp.append(maxMoves.toAnsiString());
		maxMovesEditText.setString(temp);

		try
		{
			maxMovesPerPlayer = std::stoi(temp);
			moveTimerWhite.setString("Moves: " + ((maxMovesPerPlayer - (movesMade / 2) > 0) ? std::to_string(maxMovesPerPlayer - (movesMade / 2)) : "0"));
			moveTimerBlack.setString("Moves: " + ((maxMovesPerPlayer - (movesMade / 2) > 0) ? std::to_string(maxMovesPerPlayer - (movesMade / 2)) : "0"));
		}
		catch (const std::exception& e)
		{
			maxMovesEditText.setString("");
		}
	}
}



void game::setMoveTimeLimitEditText(sf::String moveTimeLimit)
{
	if (progress == game::NOT_STARTED)
	{
		std::string temp = moveTimeLimitEditText.getString();
		temp.append(moveTimeLimit.toAnsiString());
		moveTimeLimitEditText.setString(temp);

		try
		{
			moveTimeLimitBlack = std::stoi(temp);
			timerTextBlack.setString("Black: " + std::to_string(moveTimeLimitBlack));
		}
		catch (const std::exception& e)
		{
			moveTimeLimitEditText.setString("");
		}
	}
}
void game::setMoveTimeLimitEditText2(sf::String moveTimeLimit)
{
	if (progress == game::NOT_STARTED)
	{
		std::string temp = moveTimeLimitEditText2.getString();
		temp.append(moveTimeLimit.toAnsiString());
		moveTimeLimitEditText2.setString(temp);

		try
		{
			moveTimeLimitWhite = std::stoi(temp);
			timerTextWhite.setString("White: " + std::to_string(moveTimeLimitWhite));
		}
		catch (const std::exception& e)
		{
			moveTimeLimitEditText2.setString("");
		}
	}
}

void game::initMoveTimeLimit()
{
	//Initializes textbox move timer limits for both players
	setUpTextBox(moveTimeLimitEditText, moveTimeLimitBox, moveTimeLimitTitle, 1000, 460, "Time Limit Bl:");
	setUpTextBox(moveTimeLimitEditText2, moveTimeLimitBox2, moveTimeLimitTitle2, 1000, 540, "Time Limit Wh: ");
}

void game::initPlayerColorBtns()
{
	setUpText(p1IsBlackText, "P1 Black?", 945, 630);
	p1IsBlackBtn = new button{80, "Yes", {1085, 630}, sf::Color::Black };
	p1IsBlackBtn->setFillColor(sf::Color::White);

	auto handler = new std::function<void(sf::Event&)>
	{
		[&, this](sf::Event& e)
		{
			if (progress == game::NOT_STARTED)
			{
			isP1Black = !isP1Black;
			(isP1Black) ? p1IsBlackBtn->setText("Yes") : p1IsBlackBtn->setText("No");
			}
		}
	};
	p1IsBlackBtn->registerHandler(handler);

}

void game::initNextMove()
{
	//Placerholder for next move once it has been implemented
	setUpText(nextMoveText, "I5, I6, I7 -> G6, G7, G8", 100, 20);
}

void game::initBlackTimer()
{
	setUpTimer(timerTextBlack, 400, 715, moveTimeLimitBlack, "Black: ");
}

void game::initWhiteTimer()
{
	setUpTimer(timerTextWhite, 400, 740, moveTimeLimitWhite, "White: ");
}

void game::initMoveTimer()
{
	setUpTimer(moveTimerBlack, 625, 715, maxMovesPerPlayer, "Moves: ");
	setUpTimer(moveTimerWhite, 625, 740, maxMovesPerPlayer, "Moves: ");
}

void game::setTurnTimer() {
	if (progress == gameProgress::IN_PROGRESS) {
		//TODO timer restart is in button handlers of directionals, if we make an auto execute move we should move it there
		auto time = turnTimer.getElapsedTime();
		auto second = storedTurnSec + time.asSeconds();
		//TODO if timer is 0, end game?
		if(isBlackTurn)
			timerTextBlack.setString("Black: " + ((moveTimeLimitBlack - second) > 0 ? std::to_string(moveTimeLimitBlack - second) : "0.00"));
		if(!isBlackTurn)
			timerTextWhite.setString("White: " + ((moveTimeLimitWhite - second) > 0 ? std::to_string(moveTimeLimitWhite - second) : "0.00"));	
	}
}

void game::setMoveTimer() {
	if (progress == gameProgress::IN_PROGRESS) {
		//Alternates counting of moves
		if(isBlackTurn)
			moveTimerWhite.setString("Moves: " + ((maxMovesPerPlayer - ceil(movesMade/2.0) > 0) ? std::to_string(maxMovesPerPlayer - (int) ceil(movesMade/2.0)) : "0"));
		else
			moveTimerBlack.setString("Moves: " + ((maxMovesPerPlayer - ceil(movesMade/2.0) > 0) ? std::to_string(maxMovesPerPlayer - (int) ceil(movesMade/2.0)) : "0"));
	}
}


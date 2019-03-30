#include "game.h"
#include "gui.h"

#pragma region constants

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

#pragma endregion

#pragma region constructor_destructor

game::game() : player1IsHuman{ true }, player2IsHuman{ false }, state{ INIT_STATES[EMPTY] },
progress{ gameProgress::NOT_STARTED }, rman{ &resourceManager::instance }, storedSec{ 0 }, isBlackTurn{ true }, player1IsBlack{true}
{
	initAllEle();
}

game::game(gui* ui) : player1IsHuman{ true }, player2IsHuman{ false }, state{ INIT_STATES[EMPTY] },
	ui{ ui }, progress{ gameProgress::NOT_STARTED }, rman{ &resourceManager::instance }, storedSec{ 0 }, isBlackTurn{true}, player1IsBlack{ true }
{
	initAllEle();
}

game::~game()
{
	for (auto s : showables) {
		delete s;
	}
	delete player1;
	delete player2;
}

#pragma endregion 

#pragma region override_functions

bool game::checkClick(sf::Event &)
{
	return false;
}

void game::click(sf::Event & e)
{
	for (auto c : clickables) {
		c->click(e);
	}
}

void game::show(sf::RenderWindow & window)
{
	for (auto s : showables) {
		s->show(window);
	}
	setTimer();
	setTurnTimer();
	
	/*window.draw(log);*/
}

void game::type(sf::Event & e)
{
	for (auto t : typeables) {
		t->type(e);
	}
}

void game::backspace(sf::Event & e)
{
	for (auto t : typeables) {
		t->backspace(e);
	}
}

#pragma endregion

#pragma region ui_initiate

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
	initMaxMoves();
	initMoveTimeLimit();
	initPlayerColorBtns();
	initTurnTimer();
	initMoveCounter();
	initNextMove();
}

void game::initBoard()
{
	gameBoard = new board{ 600, 400, 300, this, state };
	showables.push_back(gameBoard);
	clickables.push_back(gameBoard);
}

void game::initTimer()
{
	timerText = new textbox{ 50, {450, 0}, sf::Color::Red, "Timer: 0:0" };
	showables.push_back(timerText);
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
	showables.push_back(startBtn);
	clickables.push_back(startBtn);
}

void game::initPauseBtn()
{
	//need turnTimer store
	pauseBtn = new button{ 100, "pause", {50, 160}, sf::Color::Red };
	auto handler = new std::function<void(sf::Event&)>
	{ 
		[&, this](sf::Event& e) 
		{
			if (!(progress & gameProgress::IN_PROGRESS))
				return;
			this->progress = gameProgress::PAUSED;
			this->storedSec += this->clock.getElapsedTime().asSeconds();
			std::cout << "storedTurnSec" << turnTimer.getElapsedTime().asSeconds();
			this->storedTurnSec += turnTimer.getElapsedTime().asSeconds();
			std::cout << "Game is paused." << std::endl << std::endl;
		}
	};
	pauseBtn->registerHandler(handler);
	showables.push_back(pauseBtn);
	clickables.push_back(pauseBtn);
}

void game::initStopBtn()
{
	stopBtn = new button{ 100, "stop", {50, 220}, sf::Color::Red };
	auto handler = new std::function<void(sf::Event&)>
	{
		[&, this](sf::Event& e)
		{
			stopGame();
		}
	};

	stopBtn->registerHandler(handler);
	showables.push_back(stopBtn);
	clickables.push_back(stopBtn);
}

void game::initUndoBtn()
{
	//need to update for my state variables
	undoBtn = new button{ 100, "undo", {50, 280}, sf::Color::Red };
	auto handler = new std::function<void(sf::Event&)>{ 
		[&](sf::Event& e) 
		{
			undoGame();
		}
	};
	undoBtn->registerHandler(handler);
	showables.push_back(undoBtn);
	clickables.push_back(undoBtn);
}

void game::initResetBtn() {
	resetBtn = new button{ 100, "reset", {50, 340}, sf::Color::Red };
	auto handler = new std::function<void(sf::Event&)>{ 
		[&](sf::Event& e) 
		{
			resetGame();
		}
	};
	resetBtn->registerHandler(handler);
	showables.push_back(resetBtn);
	clickables.push_back(resetBtn);
}

void game::initboardSetupBtn() {
	boardSetupLabel = new textbox{ 25, {850, 50}, sf::Color::Red, "Choose board initial setup: " };
	standardBtn = new button{ 70, "Standard", {1000, 100}, sf::Color{ 165, 104, 24 } };
	germanDaisyBtn = new button{ 70, "German Daisy", {1000, 140}, sf::Color{ 165, 104, 24 } };
	belgainDaisyBtn = new button{ 70, "Belgain Daisy", {1000, 180}, sf::Color{ 165, 104, 24 } };
	standardBtn->getBackground().setSize({ 150, 35 });
	germanDaisyBtn->getBackground().setSize({ 150, 35 });
	belgainDaisyBtn->getBackground().setSize({ 150, 35 });
	auto handlerGen = [this](const std::string& s) {
		return new std::function<void(sf::Event&)>{ 
			[&](sf::Event& e) 
			{
				if (progress != gameProgress::NOT_STARTED)
					return;
				state = std::bitset<128U>{ s };
				gameBoard->setState(state);
			}
		};
	};
	standardBtn->registerHandler(handlerGen(INIT_STATES[STANDARD]));
	germanDaisyBtn->registerHandler(handlerGen(INIT_STATES[GERMAN_DAISY]));
	belgainDaisyBtn->registerHandler(handlerGen(INIT_STATES[BELGAIN_DAISY]));
	showables.push_back(boardSetupLabel);
	showables.push_back(standardBtn);
	showables.push_back(germanDaisyBtn);
	showables.push_back(belgainDaisyBtn);
	clickables.push_back(standardBtn);
	clickables.push_back(germanDaisyBtn);
	clickables.push_back(belgainDaisyBtn);
}

void game::initPlayerChangeBtn() {
	playerChangeLabel = new textbox{ 25, {950, 250}, sf::Color::Red, "Change players: " };
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
	showables.push_back(playerChangeLabel);
	showables.push_back(player1ChangeBtn);
	showables.push_back(player2ChangeBtn);
	clickables.push_back(player1ChangeBtn);
	clickables.push_back(player2ChangeBtn);
}

void game::initScore()
{
	blackLostText = new textbox{ 25, {520, 680}, sf::Color::Red, "Black Lost: 0" };
	whiteLostText = new textbox{ 25, {520, 90}, sf::Color::Red, "White Lost: 0" };
	showables.push_back(blackLostText);
	showables.push_back(whiteLostText);
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
			
			
			move(i);
			
		} };
		moveBtn[i]->registerHandler(handler);
		showables.push_back(moveBtn[i]);
		clickables.push_back(moveBtn[i]);
	}
}

void game::initMaxMoves()
{
	maxMovesTitle = new textbox{ 25, {1000, 380}, sf::Color::Red, "Move Limit: " };
	maxMovesEditText = new editText{ {170, 35}, {1000, 420}, sf::Color::Black, sf::Color{165,104,24} };
	maxMovesEditText->setNumberOnly(true);
	showables.push_back(maxMovesTitle);
	showables.push_back(maxMovesEditText);
	clickables.push_back(maxMovesEditText);
	typeables.push_back(maxMovesEditText);
}

void game::initMoveTimeLimit()
{
	moveTimeLimitTitle = new textbox{ 25, {1000, 460}, sf::Color::Red, "Time Limit Bl:" };
	moveTimeLimitEditText = new editText{ {170, 35}, {1000, 500}, sf::Color::Black, sf::Color{165,104,24} };
	moveTimeLimitEditText->setNumberOnly(true);
	showables.push_back(moveTimeLimitTitle);
	showables.push_back(moveTimeLimitEditText);
	clickables.push_back(moveTimeLimitEditText);
	typeables.push_back(moveTimeLimitEditText);

	moveTimeLimitTitle2 = new textbox{ 25, {1000, 540}, sf::Color::Red, "Time Limit Wh: " };
	moveTimeLimitEditText2 = new editText{ {170, 35}, {1000, 580}, sf::Color::Black, sf::Color{165,104,24} };
	moveTimeLimitEditText2->setNumberOnly(true);
	showables.push_back(moveTimeLimitTitle2);
	showables.push_back(moveTimeLimitEditText2);
	clickables.push_back(moveTimeLimitEditText2);
	typeables.push_back(moveTimeLimitEditText2);
}

void game::initPlayerColorBtns()
{
	p1IsBlackText = new textbox{ 25, {945, 630}, sf::Color::Red, "P1 Black?" };
	player1IsBlackBtn = new button{ 80, "Yes", {1085, 630}, sf::Color::Black };
	player1IsBlackBtn->setFillColor(sf::Color::White);

	auto handler = new std::function<void(sf::Event&)>
	{
		[&, this](sf::Event& e)
		{
			if (progress == game::NOT_STARTED)
			{
				player1IsBlack = !player1IsBlack;
				(player1IsBlack) ? player1IsBlackBtn->setText("Yes") : player1IsBlackBtn->setText("No");
			}
		}
	};
	player1IsBlackBtn->registerHandler(handler);
	showables.push_back(p1IsBlackText);
	showables.push_back(player1IsBlackBtn);
	clickables.push_back(player1IsBlackBtn);
}

void game::initNextMove()
{
	//Placerholder for next move once it has been implemented
	nextMoveText = new textbox{ 25, { 100, 20}, sf::Color::Red,  "I5, I6, I7 -> G6, G7, G8" };
}

void game::initTurnTimer()
{
	timerTextBlack = new textbox{ 25, { 400, 715}, sf::Color::Red,  ("Black: " + std::to_string(moveTimeLimitBlack)) };
	showables.push_back(timerTextBlack);
	timerTextWhite = new textbox{ 25, { 400, 740}, sf::Color::Red,  ("White: " + std::to_string(moveTimeLimitWhite)) };
	showables.push_back(timerTextWhite);
}

void game::initMoveCounter()
{
	moveCounterBlack = new textbox{ 25, { 625, 715}, sf::Color::Red,  ("Moves: " + std::to_string(maxMovesPerPlayer)) };
	moveCounterWhite = new textbox{ 25, { 625, 740}, sf::Color::Red,  ("Moves: " + std::to_string(maxMovesPerPlayer)) };
	showables.push_back(moveCounterBlack);
	showables.push_back(moveCounterWhite);
}

#pragma endregion

#pragma region game_state_setters

void game::startGame() {
	if (!(progress & (gameProgress::NOT_STARTED | gameProgress::PAUSED)) 
		|| state == std::bitset<128U>{INIT_STATES[EMPTY]}) {
		return;
	}
	if (gameProgress::NOT_STARTED == progress) {
		extractPropertyNDisplay(moveCounterBlack, maxMovesEditText, maxMovesPerPlayer, "Moves: ");
		extractPropertyNDisplay(moveCounterWhite, maxMovesEditText, maxMovesPerPlayer, "Moves: ");
		extractPropertyNDisplay(timerTextBlack, moveTimeLimitEditText, moveTimeLimitBlack, "Black: ");
		extractPropertyNDisplay(timerTextWhite, moveTimeLimitEditText2, moveTimeLimitWhite, "White: ");
		if (!player1IsHuman) {
			player1 = new automata{};
		}
		if (!player2IsHuman) {
			player2 = new automata{};
		}
		gameState toBeSaved = { state, storedSec, isBlackTurn };
		history.push(toBeSaved);
	}
	progress = gameProgress::IN_PROGRESS;
	if (!player1IsHuman || !player2IsHuman) {
		automataMove();
	}

	clock.restart();
	turnTimer.restart();
	std::cout << "Game START." << std::endl << std::endl;
}

void game::stopGame()
{
	if (!(progress & (gameProgress::IN_PROGRESS | gameProgress::PAUSED | gameProgress::AI_SEARCHING)))
		return;
	
	this->storedSec += this->clock.getElapsedTime().asSeconds();
	this->storedTurnSec += turnTimer.getElapsedTime().asSeconds();
	std::cout << "Total time elapsed: " << clock.getElapsedTime().asSeconds() << std::endl;
	std::cout << "Game has stopped. " << std::endl << std::endl;
	// 1. Check score
	auto scores = logic::getScoreFromState(state);

	// 2. Sum time taken by each player
	double blackTotalTime = 0.0;
	double whiteTotalTime = 0.0;

	auto lastTurn = history.top();
	history.pop();
	while (!history.empty()) {
		gameState thisTurn = history.top();
		if (lastTurn.isBlackTurn) {
			blackTotalTime += (lastTurn.storedSec - thisTurn.storedSec);
		}
		else {
			whiteTotalTime += (lastTurn.storedSec - thisTurn.storedSec);
		}
		lastTurn = thisTurn;
		history.pop();
	}

	// 3. Compare time
	std::cout << "Black lost: " << scores.x << " - White lost: " << scores.y << std::endl;
	std::cout << "Black took " << blackTotalTime << " seconds to move." << std::endl;
	std::cout << "White took " << whiteTotalTime << " seconds to move." << std::endl;
	bool isBlackWin = scores.x == scores.y ? blackTotalTime < whiteTotalTime : scores.x < scores.y;
	std::cout << (isBlackWin ? "Black" : "White") << " wins!" << std::endl;
	this->progress = isBlackWin ? gameProgress::BLACK_WINNED : gameProgress::WHITE_WINNED;
	// Set base states so game cannot be restarted without reset.
	history = {};
	state = std::bitset<128U>{ INIT_STATES[EMPTY] };
}

void game::undoGame() {
	if (progress != gameProgress::IN_PROGRESS || history.size() < 2)
		return;
	history.pop();
	gameState lastState = history.top();
	state = lastState.state;
	gameBoard->setState(state);
	setScoreFromState(state);
	storedSec = lastState.storedSec;
	storedTurnSec = 0;
	isBlackTurn = !lastState.isBlackTurn;
	clock.restart();
	turnTimer.restart();
	progress = gameProgress::PAUSED;
	std::cout << "Undo last move." << std::endl << std::endl;
}

void game::resetGame() {
	if (progress == gameProgress::NOT_STARTED)
		return;
	// Here's the test
	progress = gameProgress::NOT_STARTED;
	history = {};
	state = std::bitset<128U>{ INIT_STATES[EMPTY] };
	gameBoard->setState(state);
	gameBoard->unSelectAll();
	selectedIndex = {};
	storedSec = 0;
	isBlackTurn = true;
	timerText->setText("Timer: 0:0");
	timerTextBlack->setText("Black: 0.0");
	timerTextWhite->setText("White: 0.0");
	blackLostText->setText("Black Lost: 0");
	whiteLostText->setText("White Lost: 0");
	maxMovesEditText->setisEnbled(true);
	moveTimeLimitEditText->setisEnbled(true);
	moveTimeLimitEditText2->setisEnbled(true);
	std::cout << "Game is reseted." << std::endl << std::endl;
}

void game::move(unsigned short direction) {
	if (progress != gameProgress::IN_PROGRESS)
		return;
	if (selectedIndex.size() == 0)
		return;
	bool moveType = isSideMove(direction);
	bool valid = moveType ? isSideMoveValid(direction) : isInlineValid(direction);
	if (valid) {
		auto action = makeAction(direction, moveType);
		std::cout << "player " << (isBlackTurn ? "1(Black)" : "2(White)") << "is moving by action: " << std::endl;
		std::cout << "count: " << action.count << " index: " << action.index << " direction: " << action.direction << std::endl;
		gameBoard->unSelectAll();
		selectedIndex.clear();
		auto tempState = logic::move(state, action, isBlackTurn);
		
		nextState(tempState);
		if (!player1IsHuman || !player2IsHuman) {
			automataMove();
		}
	}
}

void game::automataMove() {

	if (gameProgress::IN_PROGRESS != progress)
		return;

	//player 2 move
	if (isBlackTurn ^ player1IsBlack) {
		if (!player2IsHuman) {
			ui->asyncAwait<std::pair<logic::action, std::bitset<128U>>>([&] {

				progress = (gameProgress::AI_SEARCHING | gameProgress::IN_PROGRESS);
				auto actionState = player2->getBestMove(state, isBlackTurn, maxMovesPerPlayer * 2 - movesMade,
					player1IsBlack ? moveTimeLimitWhite : moveTimeLimitBlack);

				return actionState;
			}, [&](std::pair <logic::action, std::bitset<128U>> actionState) {
				if (!(gameProgress::AI_SEARCHING & progress))
					return;
				progress = gameProgress::IN_PROGRESS;
				nextState(actionState.second);
				automataMove();
			});
		}
	}
	//player 1 move
	else {
		if (!player1IsHuman) {
			ui->asyncAwait<std::pair<logic::action, std::bitset<128U>>>([&] {

				progress = (gameProgress::AI_SEARCHING | gameProgress::IN_PROGRESS);
				auto actionState = player1->getBestMove(state, isBlackTurn, maxMovesPerPlayer * 2 - movesMade,
					player1IsBlack ? moveTimeLimitBlack : moveTimeLimitWhite);

				return actionState;
			}, [&](std::pair <logic::action, std::bitset<128U>> actionState) {
				if (!(gameProgress::AI_SEARCHING & progress))
					return;
				progress = gameProgress::IN_PROGRESS;
				nextState(actionState.second);
				automataMove();
			});
		}
	}
}

void game::nextState(std::bitset<128U> state) {
	++movesMade;
	setMoveCount();
	isBlackTurn = !isBlackTurn;
	turnTimer.restart();
	this->state = state;
	gameBoard->setState(state);
	setScoreFromState(state);
	gameState toBeSaved = { state, storedSec + clock.getElapsedTime().asSeconds(), !isBlackTurn };
	std::cout << "Time used: " << toBeSaved.storedSec - history.top().storedSec << " seconds." << std::endl << std::endl;
	history.push(toBeSaved);
	if (isBlackTurn) {
		timerTextWhite->setText("White: 0");
	}
	else {
		timerTextBlack->setText("Black: 0");
	}
}

#pragma endregion

#pragma region helpers

void game::setScoreFromState(std::bitset<128U>& state) {
	auto scores = logic::getScoreFromState(state);
	blackLostText->setText("Black Lost: " + std::to_string(scores.x));
	whiteLostText->setText("White Lost: " + std::to_string(scores.y));
}

void game::extractPropertyNDisplay(textbox * text, editText * editText, int& property, std::string label)
{
	auto value = editText->getText();
	if (value == "" || value.length() > 8) {
		value = "0";
	}
	auto content = (label + value);
	property = std::stoi(value);
	text->setText(content);
	editText->setisEnbled(false);
}

#pragma endregion

#pragma region getters_setters

bool game::getIsBlackTurn() {
	return isBlackTurn;
}

int game::getProgress()
{
	return progress;
}

#pragma endregion

#pragma region ui_updater

void game::setTimer() {
	if (progress & gameProgress::IN_PROGRESS) {
		//TODO also do my other timers
		auto time = clock.getElapsedTime();
		auto second = time.asSeconds();
		float tempSec = storedSec + second;
		int min = static_cast<int>(tempSec) / 60;
		int sec = static_cast<int>(tempSec) % 60;
		timerText->setText("Timer: " + std::to_string(min) + ":" + std::to_string(sec));
	}
}

void game::setTurnTimer() {
	if (progress & gameProgress::IN_PROGRESS) {
		//TODO timer restart is in button handlers of directionals, if we make an auto execute move we should move it there
		auto time = turnTimer.getElapsedTime();
		auto second = storedTurnSec + time.asSeconds();
		//TODO if timer is 0, end game?
		if (isBlackTurn) {
			auto timeleft = moveTimeLimitBlack - second;
			timerTextBlack->setText("Black: " + (timeleft > 0 ? std::to_string(timeleft) : "0"));
		} else {
			auto timeleft = moveTimeLimitWhite - second;
			timerTextWhite->setText("White: " + (timeleft > 0 ? std::to_string(timeleft) : "0"));
		}
	}
}

void game::setMoveCount() {
	if (progress & gameProgress::IN_PROGRESS) {
		//Alternates counting of moves
		auto moveLeft = static_cast<int>(maxMovesPerPlayer - ceil(movesMade / 2.0));
		if (isBlackTurn) {
			moveCounterBlack->setText("Moves: " + (moveLeft > 0 ? std::to_string(moveLeft) : "0"));
		}
		else {
			moveCounterWhite->setText("Moves: " + (moveLeft > 0 ? std::to_string(moveLeft) : "0"));
		}
	}
}

#pragma endregion

#pragma region uiMoveLogic

logic::action game::makeAction(unsigned short direction, bool isSideMove) {
	if (isSideMove) {
		int drrCcw1 = (direction + 5) % 6;
		bool isHighCcw1 = logic::CLOCKWISE_RIGHT[direction][1];
		int iCcw1 = isHighCcw1 ? *(selectedIndex.end() - 1) : *selectedIndex.begin();
		int iCcw1Next = logic::MOVE_TABLE[iCcw1][drrCcw1];
		for (int is : selectedIndex) {
			if (iCcw1Next == is)
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

bool game::trySelect(int index) {
	if (isBlackTurn ^ player1IsBlack) {
		if (!player2IsHuman)
			return false;
	}
	else {
		if (!player1IsHuman)
			return false;
	}

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

#pragma endregion
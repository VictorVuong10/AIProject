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


const std::string game::INIT_STATES[3] = { STANDARD_STR, GERMAN_DAISY_STR, BELGAIN_DAISY_STR };


game::game()
{
}

game::game(bool player1IsHuman, bool player2IsHuman, initialState initialState, gui* ui)
	: player1IsHuman{ player1IsHuman }, player2IsHuman{ player2IsHuman }, state{ INIT_STATES[initialState] },
	ui{ ui }, progress{ gameProgress::NOT_STARTED }, rman{ &resourceManager::instance }, storedSec{ 0 }, isBlackTurn{true}
{
	initAllEle();
}


bool game::checkClick(sf::Event &)
{
	return false;
}

void game::click(sf::Event & e)
{
	if (progress == gameProgress::IN_PROGRESS) {
		gameBoard->click(e);
	}
	startBtn->click(e);
	pauseBtn->click(e);
	undoBtn->click(e);
	for (auto b : moveBtn) {
		b->click(e);
	}
}

void game::show(sf::RenderWindow & window)
{
	startBtn->show(window);
	gameBoard->show(window);
	pauseBtn->show(window);
	undoBtn->show(window);
	window.draw(blackLostText);
	window.draw(whiteLostText);
	setTimer();
	window.draw(timerText);
	for (auto b : moveBtn) {
		b->show(window);
	}
	/*window.draw(log);*/
}


game::~game()
{
}

void game::initAllEle()
{
	initBoard();
	initTimer();
	initStartBtn();
	initPauseBtn();
	initUndoBtn();
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
	timerText.setString("Timer: 000:00");
	timerText.setFillColor(sf::Color::Red);
	timerText.setCharacterSize(24);
}

void game::initStartBtn()
{
	startBtn = new button{ 100, "start", {50, 100}, sf::Color::Red };
	auto handler = new std::function<void(sf::Event&)>
	{ 
		[&, this](sf::Event& e) 
		{
			if (gameProgress::NOT_STARTED == progress) {
				gameState toBeSaved = { player1IsHuman, player2IsHuman, state, storedSec + clock.getElapsedTime().asSeconds(), isBlackTurn };
				history.push(toBeSaved);
			}
			this->progress = gameProgress::IN_PROGRESS;
			this->clock.restart();
		}
	};
	startBtn->registerHandler(handler);
}

void game::initPauseBtn()
{
	pauseBtn = new button{ 100, "pause", {50, 160}, sf::Color::Red };
	auto handler = new std::function<void(sf::Event&)>
	{ 
		[&, this](sf::Event& e) 
		{
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
			gameBoard->setState(lastState.state);
			storedSec = lastState.storedSec;
			isBlackTurn = lastState.isBlackTurn;
			clock.restart();
		}
	};
	undoBtn->registerHandler(handler);
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
		int hour = static_cast<int>(tempSec) / 60;
		int sec = static_cast<int>(tempSec) % 60;
		auto tempHourStr = std::to_string(hour);
		auto tempSecStr = std::to_string(sec);
		auto hourStr = std::string(3 - tempHourStr.length(), '0').append(tempHourStr);
		auto secStr = std::string(2 - tempSecStr.length(), '0').append(tempSecStr);
		timerText.setString("Timer: " + hourStr + ":" + secStr);
	}
}

std::bitset<128U> game::move(int direction) {
	return std::bitset<128U>{};
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
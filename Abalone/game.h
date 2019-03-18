#pragma once

#include "IShowable.h"
#include "IAwaitable.h"
#include "resourceManager.h"
#include <bitset>
#include <set>
#include <stack>
#include "button.h"
#include "board.h"

class gui;
class game : public IShowable, public virtual IClickable
{
	typedef struct {
		bool player1IsHuman;
		bool player2IsHuman;
		std::bitset<128U> state;
		float storedSec;
		bool isBlackTurn;
	} gameState;

public:
	static const std::string STANDARD_STR;
	static const std::string GERMAN_DAISY_STR;
	static const std::string BELGAIN_DAISY_STR;

	static const std::string INIT_STATES[3];

	typedef enum {
		STANDARD = 0,
		GERMAN_DAISY = 1,
		BELGAIN_DAISY = 2
	} initialState;

	typedef enum {
		NOT_STARTED = 0,
		IN_PROGRESS = 1,
		PAUSED = 1 << 1,
		AI_SEARCHING =  1 << 2,
		PLAYER1_WINNED = 1 << 3,
		PLAYER2_WINNED = 1 << 4
	} gameProgress;

	game();
	game(bool player1IsHuman, bool player2IsHuman, initialState initialState, gui* gui);

	void registerHandler(handler h) = delete;
	void removeHandler(handler h) = delete;
	bool checkClick(sf::Event&) override;
	void click(sf::Event&) override;
	void show(sf::RenderWindow& window) override;
	
	bool getIsBlackTurn();

	bool trySelect(int index);

	~game();

private:
	std::stack<gameState> history;
	std::vector<int> selectedIndex;
	gui* ui;
	resourceManager* rman;
	board* gameBoard;
	button* startBtn;
	button* pauseBtn;
	button* undoBtn;
	button* moveBtn[6];
	sf::Text timerText;
	sf::Text blackLostText;
	sf::Text whiteLostText;
	sf::Clock clock;
	bool player1IsHuman;
	bool player2IsHuman;
	std::bitset<128U> state;
	gameProgress progress;
	bool isBlackTurn;
	//this is not the real game time. Use storeSec plus clock time.
	float storedSec;

	void initAllEle();
	void initBoard();
	void initTimer();
	void initStartBtn();
	void initPauseBtn();
	void initUndoBtn();
	void initScore();
	void initMoveBtn();
	void initLog();
	void setTimer();

	std::bitset<128U> move(int direction);

	void nextState(std::bitset<128U> state);

	

};


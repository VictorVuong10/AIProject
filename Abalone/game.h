#pragma once

#include "IShowable.h"
#include "resourceManager.h"
#include <bitset>
#include <set>
#include <stack>
#include <random>
#include "textbox.h"
#include "editText.h"
#include "button.h"
#include "board.h"
#include "logic.h"
#include "ITypeable.h"
#include "automata.h"

class gui;
class game : public IShowable, public virtual IClickable, public ITypeable
{
public:
	static const std::string STANDARD_STR;
	static const std::string GERMAN_DAISY_STR;
	static const std::string BELGAIN_DAISY_STR;

	static const std::string INIT_STATES[4];

	typedef enum {
		EMPTY = 0,
		STANDARD = 1,
		GERMAN_DAISY = 2,
		BELGAIN_DAISY = 3
	} initialState;

	typedef enum {
		NOT_STARTED = 0,
		IN_PROGRESS = 1,
		PAUSED = 1 << 1,
		AI_SEARCHING =  1 << 2,
		BLACK_WINNED = 1 << 3,
		WHITE_WINNED = 1 << 4
	} gameProgress;

	game();
	game(gui* gui);
	~game();

	void registerHandler(handler h) = delete;
	void removeHandler(handler h) = delete;
	bool checkClick(sf::Event&) override;
	void click(sf::Event&) override;
	void show(sf::RenderWindow& window) override;
	void type(sf::Event&) override;
	void backspace(sf::Event&) override;
	
	bool getIsBlackTurn();
	int getProgress();
	bool trySelect(int index);
	bool tryUnSelect(int index);

private:
	typedef struct {
		std::bitset<128U> state;
		float storedSec;
		bool isBlackTurn;
		//need timesec?p1isblack?maxmoves?
	} gameState;

	bool player1IsHuman;
	bool player2IsHuman;
	bool player1IsBlack;

	automata* player1;
	automata* player2;

	int movesMade;
	int moveTimeLimitBlack;
	int moveTimeLimitWhite;
	int maxMovesPerPlayer;

	bool isBlackTurn;
	std::bitset<128U> state;
	int progress;
	//this is not the real game time. Use storeSec plus clock time.
	float storedSec;
	float storedTurnSec;
	std::stack<gameState> history;
	std::vector<int> selectedIndex;

	gui* ui;
	resourceManager* rman;
	sf::Clock clock;
	sf::Clock turnTimer;

	std::vector<IShowable*> showables;
	std::vector<IClickable*> clickables;
	std::vector<ITypeable*> typeables;

	board* gameBoard;
	button* startBtn;
	button* stopBtn;
	button* pauseBtn;
	button* undoBtn;
	button* resetBtn;
	button* standardBtn;
	button* germanDaisyBtn;
	button* belgainDaisyBtn;
	button* player1ChangeBtn;
	button* player2ChangeBtn;
	button* player1IsBlackBtn;
	button* moveBtn[6];
	textbox* boardSetupLabel;
	textbox* playerChangeLabel;
	textbox* timerText;
	textbox* blackLostText;
	textbox* whiteLostText;
	textbox* p1IsBlackText;
	textbox* nextMoveText;
	textbox* timerTextBlack;
	textbox* timerTextWhite;
	textbox* moveCounterBlack;
	textbox* moveCounterWhite;
	textbox* maxMovesTitle;
	textbox* moveTimeLimitTitle;
	textbox* moveTimeLimitTitle2;
	editText* maxMovesEditText;
	editText* moveTimeLimitEditText;
	editText* moveTimeLimitEditText2;

	void initAllEle();
	void initboardSetupBtn();
	void initPlayerChangeBtn();
	void initBoard();
	void initTimer();
	void initStartBtn();
	void initStopBtn();
	void initPauseBtn();
	void initUndoBtn();
	void initResetBtn();
	void initScore();
	void initMoveBtn();
	void initMaxMoves();
	void initMoveTimeLimit();
	void initPlayerColorBtns();
	void initNextMove();
	void initTurnTimer();
	void initMoveCounter();

	void startGame();
	void undoGame();
	void resetGame();
	void stopGame();
	void move(unsigned short direction);
	void automataMove();
	void nextState(std::bitset<128U> state);

	logic::action makeAction(unsigned short direction, bool isSideMove);
	bool isSideMove(unsigned short direction);
	bool isActionValid(unsigned short, bool);
	bool isInlineValid(unsigned short direction);
	bool isSideMoveValid(unsigned short direction);

	void setTimer();
	void setMoveCount();
	void setTurnTimer();

	void extractPropertyNDisplay(textbox * text, editText * editText, int& property, std::string label);
	void setScoreFromState(std::bitset<128U>& state);
	


};


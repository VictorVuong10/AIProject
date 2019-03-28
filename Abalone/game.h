#pragma once

#include "IShowable.h"
#include "resourceManager.h"
#include <bitset>
#include <set>
#include <stack>
#include "textbox.h"
#include "editText.h"
#include "button.h"
#include "board.h"
#include "logic.h"
#include "ITypeable.h"

class gui;
class game : public IShowable, public virtual IClickable, public ITypeable
{
	typedef struct {
		std::bitset<128U> state;
		float storedSec;
		bool isBlackTurn;
		//need timesec?p1isblack?maxmoves?
	} gameState;

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
		PLAYER1_WINNED = 1 << 3,
		PLAYER2_WINNED = 1 << 4
	} gameProgress;

	game();
	game(gui* gui);

	void registerHandler(handler h) = delete;
	void removeHandler(handler h) = delete;
	bool checkClick(sf::Event&) override;
	void click(sf::Event&) override;
	void show(sf::RenderWindow& window) override;
	void type(sf::Event&) override;
	void backspace(sf::Event&) override;
	
	bool getIsBlackTurn();
	gameProgress getProgress();

	bool trySelect(int index);

	bool tryUnSelect(int index);

	void setMaxMovesEditText(sf::String maxMoves);
	void setMoveTimeLimitEditText(sf::String moveTimeLimit);
	void stopGame();
	void setMoveTimeLimitEditText2(sf::String moveTimeLimit);
	void setTurnTimer();

	~game();

private:
	bool player1IsHuman;
	bool player2IsHuman;
	bool isP1Black;

	int movesMade;
	int moveTimeLimitBlack;
	int moveTimeLimitWhite;
	int maxMovesPerPlayer;

	bool isBlackTurn;
	std::bitset<128U> state;
	gameProgress progress;
	//this is not the real game time. Use storeSec plus clock time.
	float storedSec;
	float storedTurnSec;
	std::stack<gameState> history;
	std::vector<int> selectedIndex;


	std::vector<IShowable*> showables;
	std::vector<IClickable*> clickables;
	std::vector<ITypeable*> typeables;

	gui* ui;
	resourceManager* rman;
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
	sf::Clock clock;
	

	sf::Text maxMovesTitle;
	sf::RectangleShape maxMovesBox;
	sf::Text maxMovesEditText;
	sf::Text moveTimeLimitTitle;
	sf::Text moveTimeLimitTitle2;
	sf::RectangleShape moveTimeLimitBox;
	sf::RectangleShape moveTimeLimitBox2;
	sf::Text moveTimeLimitEditText;
	sf::Text moveTimeLimitEditText2;
	sf::Text p1IsBlackText;
	sf::Text nextMoveText;
	sf::Clock turnTimer;
	sf::Text timerTextBlack;
	sf::Text timerTextWhite;
	sf::Text moveTimerBlack;
	sf::Text moveTimerWhite;

	void initMaxMoves();
	void initMoveTimeLimit();
	void initPlayerColorBtns();
	void initNextMove();
	void initBlackTimer();
	void initWhiteTimer();
	void initMoveTimer();

	void initAllEle();
	void initboardSetupBtn();
	void initPlayerChangeBtn();
	void initBoard();
	void initTimer();
	void initStartBtn();
	void initStopBtn();
	void startGame();
	void initPauseBtn();
	void initUndoBtn();
	void initResetBtn();
	void initScore();
	void initMoveBtn();
	logic::action makeAction(unsigned short direction, bool isSideMove);
	bool isSideMove(unsigned short direction);
	bool isActionValid(unsigned short, bool);
	bool isInlineValid(unsigned short direction);
	bool isSideMoveValid(unsigned short direction);
	void initLog();
	void setTimer();
	void setMoveTimer();
	void setUpText(sf::Text& textElement, std::string text, int x, int y);
	void setUpTimer(sf::Text& timer, int x, int y, int timeLimit, std::string text);
	void setUpTextBox(sf::Text& textElement, sf::RectangleShape& boxElement, sf::Text& titleElement, int x, int y, std::string title);

	//gameProgress getProgress();

	void nextState(std::bitset<128U> state);

	void setScoreFromState(std::bitset<128U> state);

};


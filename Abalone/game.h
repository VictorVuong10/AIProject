#pragma once

#include "IShowable.h"
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
	
	bool getIsBlackTurn();

	bool trySelect(int index);

	void unSelect(int index);

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
	button* resetBtn;
	button* standardBtn;
	button* germanDaisyBtn;
	button* belgainDaisyBtn;
	button* player1ChangeBtn;
	button* player2ChangeBtn;
	button* moveBtn[6];
	sf::Text boardSetupLabel;
	sf::Text playerChangeLabel;
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
	void initboardSetupBtn();
	void initPlayerChangeBtn();
	void initBoard();
	void initTimer();
	void initStartBtn();
	void startGame();
	void initPauseBtn();
	void initUndoBtn();
	void initResetBtn();
	void initScore();
	void initMoveBtn();
	void initLog();
	void setTimer();

	int MOVE_TABLE[61][6] =
	{
		//0 1ST ROW
		{-1, -1, -1, 1, 6, 5},
		//1
		{0, -1, -1, 2, 7, 6},
		//2
		{1, -1, -1, 3, 8, 7},
		//3
		{2, -1, -1, 4, 9, 8},
		//4
		{3, -1, -1, -1, 10, 9},
		//5 2ND ROW
		{-1, -1, 0, 6, 12, 11},
		//6
		{5, 0, 1, 7, 13, 12},
		//7
		{6, 1, 2, 8, 14, 13},
		//8
		{7, 2, 3, 9, 15, 14},
		//9
		{8, 3, 4, 10, 16, 15},
		//10
		{9, 4, -1, -1, 17, 16},
		//11 3RD ROW
		{-1, -1, 5, 12, 19, 18},
		//12
		{11, 5, 6, 13, 20, 19},
		//13
		{12, 6, 7, 14, 21, 20},
		//14
		{13, 7, 8, 15, 22, 21},
		//15
		{14, 8, 9, 16, 23, 22},
		//16
		{15, 9, 10, 17, 24, 23},
		//17
		{16, 10, -1, -1, 25, 24},
		//18 4TH ROW
		{-1, -1, 11, 19, 27, 26},
		//19
		{18, 11, 12, 20, 28, 27},
		//20
		{19, 12, 13, 21, 29, 28},
		//21
		{20, 13, 14, 22, 30, 29},
		//22
		{21, 14, 15, 23, 31, 30},
		//23
		{22, 15, 16, 24, 32, 31},
		//24
		{23, 16, 17, 25, 33, 32},
		//25
		{24, 17, -1, -1, 34, 33},
		//26 5TH ROW
		{-1, -1, 18, 27, 35, -1},
		//27
		{26, 18, 19, 28, 36, 35},
		//28
		{27, 19, 20, 29, 37, 36},
		//29
		{28, 20, 21, 30, 38, 37},
		//30
		{29, 21, 22, 31, 39, 38},
		//31
		{30, 22, 23, 32, 40, 39},
		//32
		{31, 23, 24, 33, 41, 40},
		//33
		{32, 24, 25, 34, 42, 41},
		//34
		{33, 25, -1, -1 , -1, 42},
		//35 6TH ROW
		{-1, 26, 27, 36, 43, -1},
		//36
		{35, 27, 28, 37, 44, 43},
		//37
		{36, 28, 29, 38, 45, 44},
		//38
		{37, 29, 30, 39, 46, 45},
		//39
		{38, 30, 31, 40, 47, 46},
		//40 
		{39, 31, 32, 41, 48, 47},
		//41
		{40, 32, 33, 42, 49, 48},
		//42
		{41, 33, 34, -1, -1, 49},
		//43 7TH ROW
		{-1, 35, 36, 44, 50, -1},
		//44
		{43, 36, 37, 45, 51, 50},
		//45
		{44, 37, 38, 46, 52, 51},
		//46
		{45, 38, 39, 47, 53, 52},
		//47
		{46, 39, 40, 48, 54, 53},
		//48
		{47, 40, 41, 49, 55, 54},
		//49
		{48, 41, 42, -1, -1, 55},
		//50 8TH ROW
		{-1, 43, 44, 51, 56, -1},
		//51
		{50, 44, 45, 52, 57, 56},
		//52
		{51, 45, 46, 53, 58, 57},
		//53
		{52, 46, 47, 54, 59, 58},
		//54
		{53, 47, 48, 55, 60, 59 },
		//55
		{54, 48, 49, -1, -1, 60},
		//56 9TH ROW
		{-1, 50, 51, 57, -1, -1},
		//57
		{56, 51, 52, 58, -1, -1},
		//58
		{57, 52, 53, 59, -1, -1 },
		//59
		{58, 53, 54, 60, -1, -1},
		//60
		{59, 54, 55, -1, -1, -1}
	};

	std::bitset<128U> move(int marbleCount, int index, int direction);

	void nextState(std::bitset<128U> state);

	

};


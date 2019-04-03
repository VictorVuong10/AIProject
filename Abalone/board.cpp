#include "board.h"
#include "game.h"


board::board(float x, float y, float size, game* game, std::bitset<128U> boardState) : _game{game}
{
	this->boardState = boardState; 
	bg = slot::buildHexagon(x, y, size, sf::Color{ 95,159,185 });
	initAllSlots(x, y, size);
}

board::~board()
{
	for (auto s : slots) {
		delete s;
	}
}

void board::initAllSlots(float x, float y, float size) {
	unsigned int slotIndex = 0;
	float slotSize = size / 9;
	float height = sqrt(pow(slotSize, 2) - pow(slotSize / 2, 2));
	for (int i = 0; i < 9; ++i) {
		unsigned int slotNumber = ROW_NUMBER_MAP[i];
		unsigned int rowDiff = board::ROW_E_NUMBER - slotNumber;
		float xinit = x - (slotNumber - 1) * slotSize;
		float ydiff = 2 * rowDiff * height;
		float yinit = i < 4 ? y - ydiff : y + ydiff;
		for (size_t j = 0; j < slotNumber; ++j) {
			//slotState is reversed again
			std::bitset<2U> slotState = boardState[slotIndex] << 1 | boardState[slotIndex + 1] << 0;

			char row = 'I' - i;

			std::string a;

			a += row;

			int cellNum;

			if (i < 5) {
				cellNum = rowDiff + 1 + j;
			}
			else {

				cellNum = j+1;
			}

			a+=std::to_string(cellNum);

			slot* s = new slot{ xinit, yinit, slotSize, a, slotState };

			

			s->registerHandler(new std::function<void(sf::Event&)>{ [&, s, slotIndex](sf::Event e) {
				auto sState = s->getState();
				if (sState.none() || sState[0] != _game->getIsBlackTurn())
					return;
				if (s->isSelected()) {
					if(_game->tryUnSelect(slotIndex >> 1))
						s->unSelect();
				} else {
					if(_game->trySelect(slotIndex >> 1))
						s->select();
				}
			} });
			slots.push_back(s);
			xinit += 2 * slotSize;
			slotIndex += 2;
		}
	}
}

void board::unSelectAll() {
	for (auto s : slots) {
		s->unSelect();
	}
}

void board::setState(std::bitset<128U> boardState)
{
	unsigned int stateIndex = 0;
	for (auto s : slots) {
		std::bitset<2U> slotState = boardState[stateIndex] << 1 | boardState[stateIndex + 1] << 0;
		s->setState(slotState);
		stateIndex += 2;
	}
}

bool board::checkClick(sf::Event & e)
{
	return bg.getBounds().contains(static_cast<float>(e.mouseButton.x), static_cast<float>(e.mouseButton.y));
}

void board::click(sf::Event & e)
{
	if (game::gameProgress::IN_PROGRESS != _game->getProgress())
		return;
	if (checkClick(e)) {
		for (auto s : slots) {
			s->click(e);
		}
	}
}

void board::show(sf::RenderWindow & window)
{
	window.draw(bg);
	for (auto slot : slots) {
		slot->show(window);
	}
}



#include "board.h"
#include "game.h"


board::board(float x, float y, float size, game* game, std::bitset<128U> boardState) : _game{game}
{
	this->boardState = boardState;
	bg = slot::buildHexagon(x, y, size, sf::Color{ 165, 104, 24 });
	initAllSlots(x, y, size);
}

board::~board()
{
	for (auto s : slots) {
		delete s;
	}
}

void board::initAllSlots(float x, float y, float size) {
	unsigned int stateIndex = 0;
	float slotSize = size / 9;
	float height = sqrt(pow(slotSize, 2) - pow(slotSize / 2, 2));
	for (int i = 0; i < 9; ++i) {
		unsigned int slotNumber = ROW_NUMBER_MAP[i];
		unsigned int rowDiff = board::ROW_E_NUMBER - slotNumber;
		float xinit = x - (slotNumber - 1) * slotSize;
		float ydiff = 2 * rowDiff * height;
		float yinit = i < 4 ? y - ydiff : y + ydiff;
		for (size_t j = 0; j < slotNumber; ++j) {
			std::bitset<2U> slotState = boardState[stateIndex] << 1 | boardState[stateIndex + 1];
			slot* s = new slot{ xinit, yinit, slotSize, slotState };
			s->registerHandler(new std::function<void(sf::Event&)>{ [&, s, stateIndex](sf::Event e) {
				if (s->getState()[0] != _game->getIsBlackTurn())
					return;
				if (s->isSelected()) {
					s->unSelect();
				}
				else {
					if(_game->trySelect(stateIndex >> 1))
						s->select();
				}
			} });
			slots.push_back(s);
			xinit += 2 * slotSize;
			stateIndex += 2;
		}
	}
}

void board::setState(std::bitset<128U> boardState)
{
	unsigned int stateIndex = 0;
	for (auto s : slots) {
		std::bitset<2U> slotState = boardState[stateIndex] << 1 | boardState[stateIndex + 1];
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



#include "slot.h"
#include <iostream>

const sf::Color slot::normalColor{ 36,123,160 };
const sf::Color slot::selectedColor{ 255,22,84 };

int slot::slotNumber = 0;

slot::slot(float x, float y, float size, std::bitset<2U> state) : marble(size * 0.8f), selected(false)
{
	setState(state);
	float r = marble.getRadius();
	marble.setPosition(x - r, y - r);

	bg = buildHexagon(x, y, size, normalColor);

	// set the text style
	sf::Font font;
	if (!font.loadFromFile("../arial.ttf")) {
		std::cout << "font not loaded" << std::endl;
	}
	else {
		//slotInd.setFont(font);
		slotInd.setString(std::to_string(slotNumber));
		++slotNumber;
		slotInd.setCharacterSize(30);
		slotInd.setPosition(x - r, y - r);
		slotInd.setFillColor(selectedColor);
	}

	//slotInd.setFont(font);


}


slot::~slot()
{
}

sf::VertexArray slot::buildHexagon(float x, float y, float size, sf::Color c) {
	sf::VertexArray hexa{ sf::TriangleFan, 6 };
	float height = sqrt(pow(size, 2) - pow(size / 2, 2));
	hexa[0].position = sf::Vector2f{ x - size, y };
	hexa[1].position = sf::Vector2f{ x - size / 2 , y - height };
	hexa[2].position = sf::Vector2f{ x + size / 2 , y - height };
	hexa[3].position = sf::Vector2f{ x + size, y };
	hexa[4].position = sf::Vector2f{ x + size / 2 , y + height };
	hexa[5].position = sf::Vector2f{ x - size / 2 , y + height };
	
	for (size_t i = 0; i < hexa.getVertexCount(); ++i) {

		hexa[i].color = c;
	}
	return hexa;
}

bool slot::checkClick(sf::Event & e)
{
	return marble.getGlobalBounds().contains(static_cast<float>(e.mouseButton.x), static_cast<float>(e.mouseButton.y));
}

void slot::show(sf::RenderWindow & window)
{

	window.draw(bg);
	if (!empty) {
		window.draw(marble);
	}
	window.draw(slotInd);
}

void slot::select()
{
	for (size_t i = 0; i < bg.getVertexCount(); ++i) {
		bg[i].color = selectedColor;
	}
	selected = true;
}

void slot::unSelect()
{
	for (size_t i = 0; i < bg.getVertexCount(); ++i) {
		bg[i].color = normalColor;
	}
	selected = false;
}

void slot::setState(std::bitset<2U> state)
{
	this->state = state;
	empty = state.none();
	if (!empty) {
		marble.setFillColor(state[0] ? sf::Color::Black : sf::Color::White );
	}
}

std::bitset<2U>& slot::getState()
{
	return state;
}

void slot::setSelected(bool b)
{
	selected = b;
}

bool slot::isSelected()
{
	return selected;
}

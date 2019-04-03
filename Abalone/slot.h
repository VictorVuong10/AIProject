#pragma once

#include "IShowable.h"
#include "IClickable.h"
#include <bitset>

class slot : public IShowable, public IClickable
{
private:
	sf::CircleShape marble;
	sf::VertexArray bg;
	std::bitset<2U> state;
	static int slotNumber;
	bool selected;
	bool empty;
public:
	static const sf::Color normalColor;
	static const sf::Color selectedColor;


	sf::Text slotInd;

	slot() = default;
	slot(float x, float y, float size, std::string row, std::bitset<2U> state = std::bitset<2U>(0));
	~slot();
	bool checkClick(sf::Event & e) override;
	void show(sf::RenderWindow & window) override;
	void select();
	void unSelect();
	void setState(std::bitset<2U> state);
	std::bitset<2U>& getState();
	void setSelected(bool b);
	bool isSelected();
	static sf::VertexArray buildHexagon(float size, float x, float y, sf::Color c);
};


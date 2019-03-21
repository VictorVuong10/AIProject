#pragma once

#include "IShowable.h"
#include "resourceManager.h"
#include <iostream>
#include "IClickable.h"

class button : public IShowable, public IClickable{
public:
	button() = default;
	button(float size, std::string text, sf::Vector2f location, sf::Color color);
	bool checkClick(sf::Event& e) override;
	sf::RectangleShape& getBackground();
	void setBackground(sf::RectangleShape bg);

	void setText(std::string);
	sf::Text * getText();
	void show(sf::RenderWindow& window) override;
	void setFillColor(sf::Color color);
private:

	sf::RectangleShape background;
	sf::Text Text;
};


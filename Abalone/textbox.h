#pragma once

#include "IShowable.h"
#include "resourceManager.h"

class textbox : public IShowable
{
public:
	textbox() = default;
	textbox(unsigned int size, sf::Vector2f location, sf::Color color = sf::Color::Black, std::string content = "");
	~textbox() = default;

	void show(sf::RenderWindow& window) override;

	std::string getText();
	void setText(std::string);

	sf::Text& getSfText();

private:
	sf::Text text;
};


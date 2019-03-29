#pragma once
#include "IClickable.h"
#include "ITypeable.h"
#include "IShowable.h"
#include "resourceManager.h"

class editText : public IShowable, public virtual IClickable, public ITypeable
{
public:
	editText() = default;
	editText(sf::Vector2f size, sf::Vector2f location, sf::Color fontColor = sf::Color::Black, sf::Color bgColor = sf::Color::White);
	editText(sf::Vector2f size, sf::Vector2f location);
	~editText() = default;

	void show(sf::RenderWindow& window) override;
	
	void type(sf::Event&) override;
	void backspace(sf::Event&) override;
	
	bool checkClick(sf::Event&) override;
	void click(sf::Event&) override;
	void registerHandler(handler h) = delete;
	void removeHandler(handler h) = delete;

	sf::RectangleShape& getBackground();
	void setBackground(sf::RectangleShape bg);

	std::string getText();
	void setText(std::string);

	sf::Text& getSfText();

	bool isNumberOnly();
	void setNumberOnly(bool b);

	bool isEnbled();
	void setisEnbled(bool b);

private:
	sf::RectangleShape bg;
	sf::Text text;
	sf::Color bgColor;
	bool focused;
	bool enbled;
	bool numberOnly;
};


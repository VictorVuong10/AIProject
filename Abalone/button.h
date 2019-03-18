#pragma once

#include "IShowable.h"
#include "resourceManager.h"
#include <iostream>
#include "IClickable.h"

class button : public IShowable, public IClickable{
public:
	//button(sf::Texture* normal, sf::Texture* clicked, std::string words, sf::Vector2f location);
	button() = default;
	button(float size, std::string text, sf::Vector2f location, sf::Color color);
	bool checkClick(sf::Event& e) override;
	sf::RectangleShape& getBackground();
	void setBackground(sf::RectangleShape bg);
	/*void setState(bool);
	bool getVar();
	sf::Sprite* getSprite();*/
	void setText(std::string);
	sf::Text * getText();
	//void show(std::vector<sf::Drawable*>& shapes, std::mutex & mtx);
	void show(sf::RenderWindow& window) override;
private:
	/*sf::Sprite normal;
	sf::Sprite clicked;
	sf::Sprite* currentSpr;*/
	sf::RectangleShape background;
	sf::Text Text;
};


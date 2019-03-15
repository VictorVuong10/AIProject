#pragma once

#include "IShowable.h"
#include "gui.h"
#include <iostream>
#include "IClickable.h"

class button : public IShowable, public IClickable{
public:
	button(sf::Texture* normal, sf::Texture* clicked, std::string words, sf::Vector2f location);
	bool checkClick(sf::Event& e) override;
	void setState(bool);
	void setText(std::string);
	bool getVar();
	sf::Sprite* getSprite();
	sf::Text * getText();
	//void show(std::vector<sf::Drawable*>& shapes, std::mutex & mtx);
	void show(sf::RenderWindow& window) override;
private:
	sf::Sprite normal;
	sf::Sprite clicked;
	sf::Sprite* currentSpr;
	sf::Text Text;
	bool current;
};


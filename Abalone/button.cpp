#include "button.h"


button::button(sf::Texture* normal, sf::Texture* clicked, std::string words, sf::Vector2f location) {
	this->normal.setTexture(*normal);
	this->clicked.setTexture(*clicked);
	this->currentSpr = &this->normal;
	current = false;
	this->normal.setPosition(location);
	this->clicked.setPosition(location);
	resourceManager &rman = resourceManager::instance;
	sf::Font &arial = rman.getFont("arial");
	Text.setFont(arial);
	Text.setString(words);
	Text.setFillColor(sf::Color::White);
	Text.setPosition(location.x + 3, location.y + 3);
	Text.setCharacterSize(30);
}
bool button::checkClick(sf::Event& e) {
	return currentSpr->getGlobalBounds().contains(static_cast<float>(e.mouseButton.x), static_cast<float>(e.mouseButton.y));
}
void button::setState(bool which) {
	current = which;
	if (current) {
		currentSpr = &clicked;
		return;
	}
	currentSpr = &normal;
}
void button::setText(std::string words) {
	Text.setString(words);
}
bool button::getVar() {
	return current;
}
sf::Sprite* button::getSprite() {
	return currentSpr;
}

sf::Text * button::getText() {
	return &Text;
}


void button::show(sf::RenderWindow& window) {
	window.draw(*currentSpr);
	window.draw(Text);
}
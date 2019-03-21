#include "button.h"


button::button(float size, std::string text, sf::Vector2f location, sf::Color color) : background{ {size, size / 2.f} }
{
	background.setPosition(location);
	background.setFillColor(color);
	resourceManager &rman = resourceManager::instance;
	sf::Font &arial = rman.getFont("arial");
	Text.setFont(arial);
	Text.setString(text);
	Text.setFillColor(sf::Color::White);
	Text.setPosition(location.x + size * 0.1f, location.y + size / 2.f * 0.05f);
	Text.setCharacterSize(static_cast<unsigned int>(size / 2 * 0.6));
}

bool button::checkClick(sf::Event& e) {
	return background.getGlobalBounds().contains(static_cast<float>(e.mouseButton.x), static_cast<float>(e.mouseButton.y));
}

sf::RectangleShape& button::getBackground() {
	return background;
}

void button::setBackground(sf::RectangleShape bg) {
	background = bg;
}

void button::setText(std::string words) {
	Text.setString(words);
}
sf::Text * button::getText() {
	return &Text;
}


void button::show(sf::RenderWindow& window) {
	window.draw(background);
	window.draw(Text);
}
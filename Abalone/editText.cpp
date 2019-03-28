#include "editText.h"

editText::editText(sf::Vector2f size, sf::Vector2f location)
	:bg{ size }, text{ }, focused{false}
{
	bg.setPosition(location);
	bg.setFillColor(sf::Color{ 181, 182, 183 });
	text.setFont(resourceManager::instance.getFont("arial"));
	text.setPosition(location);
	text.setFillColor(sf::Color::Black);
	text.setCharacterSize(static_cast<unsigned int>(size.y * 0.8));
}

void editText::show(sf::RenderWindow & window)
{
	window.draw(bg);
	window.draw(text);
}

void editText::type(sf::Event & e)
{
	if (focused) {
		std::string content = text.getString();
		content += static_cast<char>(e.text.unicode);
		text.setString(content);
	}
}

void editText::backspace(sf::Event & e)
{
	if (focused) {
		std::string content = text.getString();
		text.setString(content.substr(0, content.length() - 1));
	}
}

bool editText::checkClick(sf::Event & e)
{
	return bg.getGlobalBounds().contains(static_cast<float>(e.mouseButton.x), static_cast<float>(e.mouseButton.y));
}

void editText::click(sf::Event & e)
{
	focused = checkClick(e);
	bg.setFillColor(focused ? sf::Color::White : sf::Color{ 181, 182, 183 });
}

sf::RectangleShape & editText::getBackground()
{
	return bg;
}

void editText::setBackground(sf::RectangleShape bg)
{
	bg = std::move(bg);
}

sf::Text & editText::getText()
{
	return text;
}

void editText::setText(std::string s)
{
	text.setString(s);
}

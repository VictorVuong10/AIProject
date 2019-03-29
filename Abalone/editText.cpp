#include "editText.h"

editText::editText(sf::Vector2f size, sf::Vector2f location, sf::Color fontColor, sf::Color bgColor)
	:bg{ size }, text{ }, bgColor{ bgColor }, focused{ false }, enbled{ true }, numberOnly{ false }
{
	bg.setPosition(location);
	bg.setFillColor(sf::Color{ 181, 182, 183 });
	text.setFont(resourceManager::instance.getFont("arial"));
	text.setPosition(location);
	text.setFillColor(fontColor);
	text.setCharacterSize(static_cast<unsigned int>(size.y * 0.8));
}

editText::editText(sf::Vector2f size, sf::Vector2f location)
	:editText{ size , location, sf::Color::Black, sf::Color::White }
{}

void editText::show(sf::RenderWindow & window)
{
	window.draw(bg);
	window.draw(text);
}

void editText::type(sf::Event & e)
{
	if (focused && (!numberOnly || (e.text.unicode > 47 && e.text.unicode < 58))) {
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
	if (enbled) {
		focused = checkClick(e);
		bg.setFillColor(focused ? bgColor : sf::Color{ 181, 182, 183 });
	}
}

sf::RectangleShape & editText::getBackground()
{
	return bg;
}

void editText::setBackground(sf::RectangleShape bg)
{
	bg = std::move(bg);
}

std::string editText::getText()
{
	return text.getString();
}

void editText::setText(std::string s)
{
	text.setString(s);
}

sf::Text & editText::getSfText()
{
	return text;
}

bool editText::isNumberOnly()
{
	return numberOnly;
}

void editText::setNumberOnly(bool b)
{
	numberOnly = b;
}

bool editText::isEnbled()
{
	return enbled;
}

void editText::setisEnbled(bool b)
{
	enbled = b;
	if (!enbled) {
		focused = false;
		bg.setFillColor(sf::Color{ 181, 182, 183 });
	}
}

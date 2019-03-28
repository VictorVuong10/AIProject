#include "textbox.h"

textbox::textbox(unsigned int size, sf::Vector2f location, sf::Color color, std::string content)
{
	text.setFont(resourceManager::instance.getFont("arial"));
	text.setPosition(location);
	text.setFillColor(color);
	text.setCharacterSize(size);
	text.setString(content);
}

void textbox::show(sf::RenderWindow & window)
{
	window.draw(text);
}

sf::Text & textbox::getText()
{
	return text;
}

void textbox::setText(std::string s)
{
	text.setString(s);
}

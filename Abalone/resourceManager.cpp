#include "resourceManager.h"

resourceManager resourceManager::instance{};

resourceManager::resourceManager() : fontMap{}, textureMap{}
{
}

bool resourceManager::loadFont(std::string name, std::string path)
{
	sf::Font* newFont = new sf::Font{};
	bool res = newFont->loadFromFile(path);
	fontMap[name] = newFont;
	return res;
}

bool resourceManager::loadTexture(std::string name, std::string path)
{
	sf::Texture* newTexture = new sf::Texture{};
	bool res = newTexture->loadFromFile(path);
	textureMap[name] = newTexture;
	return res;
}

sf::Font & resourceManager::getFont(std::string name)
{
	if (fontMap.find(name) == fontMap.end()) {
		throw "no resource";
	}

	return *fontMap[name];
}

sf::Texture & resourceManager::getTexture(std::string name)
{
	if (textureMap.find(name) == textureMap.end()) {
		throw "no resource";
	}

	return *textureMap[name];
}

void resourceManager::removeFont(std::string name)
{
	if (fontMap.find(name) == fontMap.end()) {
		return;
	}
	delete fontMap[name];
	fontMap.erase(name);
}

void resourceManager::removeTexture(std::string name)
{
	if (textureMap.find(name) == textureMap.end()) {
		return;
	}
	delete textureMap[name];
	textureMap.erase(name);
}

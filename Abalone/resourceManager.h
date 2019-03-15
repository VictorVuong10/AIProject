#pragma once

#include <unordered_map>
#include "IShowable.h"

class resourceManager
{
private:
	std::unordered_map<std::string, sf::Font*> fontMap;
	
	std::unordered_map<std::string, sf::Texture*> textureMap;

	resourceManager();

public:
	resourceManager(const resourceManager&) = delete;
	void operator=(const resourceManager&) = delete;

	bool loadFont(std::string name, std::string path);
	bool loadTexture(std::string name, std::string path);

	sf::Font& getFont(std::string name);
	sf::Texture& getTexture(std::string name);

	void removeFont(std::string name);
	void removeTexture(std::string name);

	static resourceManager instance;

	~resourceManager() = default;
};


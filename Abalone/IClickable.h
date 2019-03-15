#pragma once
#include "IShowable.h"
#include <functional>
#include <vector>

class IClickable{
public:
	typedef std::function<void(sf::Event&)>* handler;
	virtual bool checkClick(sf::Event&) = 0;
	virtual void click(sf::Event&);
	void registerHandler(handler h);
	void removeHandler(handler h);
	~IClickable();
protected:
	std::vector<handler> handlers;

};

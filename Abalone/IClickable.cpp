#include "IClickable.h"

void IClickable::click(sf::Event & e)
{
	if (checkClick(e)) {
		for (auto h : handlers) {
			(*h)(e);
		}
	}
}

void IClickable::registerHandler(handler h)
{
	handlers.push_back(h);
}

void IClickable::removeHandler(handler h)
{
	handlers.erase(std::find(handlers.begin(), handlers.end(), h));
}

IClickable::~IClickable() {
	for (auto h : handlers) {
		delete h;
	}
}
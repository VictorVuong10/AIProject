#pragma once

#include "IShowable.h"

class ITypeable {
public:
	virtual void type(sf::Event&) = 0;
	virtual void backspace(sf::Event&) = 0;
	~ITypeable() = default;
};

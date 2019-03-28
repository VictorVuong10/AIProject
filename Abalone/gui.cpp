#include "gui.h"

gui* gui::cur = nullptr;

gui::gui() : callback{}, window{ sf::VideoMode(1200, 800), "ABALONE!" }
{
	window.setFramerateLimit(30);
	cur = this;
}

gui::gui(game & game) : callback{}, window{ sf::VideoMode(1200, 800), "ABALONE" }
{
	window.setFramerateLimit(30);
}


gui::~gui()
{
	delete _game;
}

void gui::start()
{
	bool inMaxMoves = false;
	bool inMoveTimeLimit = false;
	bool inMoveTimeLimit2 = false;

	using namespace std;
	if (!loadAllResource()) {
		return;
	}

	_game = new game{ this };

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonPressed) {
				_game->click(event);
			}
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				if (event.mouseButton.x > 1000 && event.mouseButton.x < 1170)
				{
					if (event.mouseButton.y < 455 && event.mouseButton.y > 420)
					{
						inMaxMoves = true;
						inMoveTimeLimit = false;
						inMoveTimeLimit2 = false;
					}
					if (event.mouseButton.y < 535 && event.mouseButton.y > 500)
					{
						inMaxMoves = false;
						inMoveTimeLimit = true;
						inMoveTimeLimit2 = false;
					}
					if (event.mouseButton.y < 610 && event.mouseButton.y > 580)
					{
						inMaxMoves = false;
						inMoveTimeLimit = false;
						inMoveTimeLimit2 = true;
					}
				}
			}
			// && _game.progress == game::NOT_STARTED
			if (event.type == sf::Event::TextEntered)
			{
				if (event.text.unicode == '\b') {
					_game->backspace(event);
				}
				else if (event.text.unicode < 128)
				{
					_game->type(event);
					if(inMaxMoves)
						_game->setMaxMovesEditText(event.text.unicode);
					if(inMoveTimeLimit)
						_game->setMoveTimeLimitEditText(event.text.unicode);
					if(inMoveTimeLimit2)
						_game->setMoveTimeLimitEditText2(event.text.unicode);
				}
			}
		}
		executeCallback();


		window.clear();
		_game->show(std::ref(window));
		window.display();
	}
}

void gui::safePush(std::function<void()> cb) {
	std::unique_lock<std::mutex> lock{cbMtx};
	callback.push(cb);
}

void gui::executeCallback() {
	while (!callback.empty()) {
		std::unique_lock<std::mutex> lock{cbMtx};
		std::function<void()> cb = std::move(callback.front());
		callback.pop();
		lock.unlock();
		cb();
	}
}

bool gui::loadAllResource() {
	resourceManager& rman = resourceManager::instance;
	return rman.loadFont("arial", "../arial.ttf");
}

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
			if (event.type == sf::Event::KeyPressed) {

			}
			if (event.type == sf::Event::MouseButtonPressed) {
				_game->click(event);
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

#include "gui.h"

gui* gui::cur = nullptr;

gui::gui() : callback{}, window{ sf::VideoMode(1200, 800), "SFML works!" }
{
	window.setFramerateLimit(30);
	cur = this;
}

gui::gui(game & game) : callback{}, window{ sf::VideoMode(1200, 800), "SFML works!" }
{
	window.setFramerateLimit(30);
}


gui::~gui()
{
	delete _game;
}

void gui::startBtn()
{
	using namespace std;
	if (!loadAllResource()) {
		return;
	}
	resourceManager &rman = resourceManager::instance;
	sf::Font &arial = rman.getFont("arial");
	sf::Text text;
	text.setFont(arial);
	text.setString("hello");
	text.setFillColor(sf::Color::Red);
	text.setCharacterSize(24);
	text.setPosition(900, 700);
/*
	std::bitset<128U> state{ game::GERMAN_DAISY_STR };

	board b{ 500, 500, 300, state };
*/
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
				std::cout << "mouse x: " << event.mouseButton.x << std::endl;
				std::cout << "mouse y: " << event.mouseButton.y << std::endl;
				//btn->click(event);
				_game->click(event);
			}
		}
		auto now = std::chrono::system_clock::now();
		time_t nowt = std::chrono::system_clock::to_time_t(now);
		char ts[100];
		ctime_s(ts, 100, &nowt);
		text.setString(ts);

		executeCallback();


		window.clear();
		//btn->show(std::ref(window));
		_game->show(std::ref(window));
		window.draw(text);
		window.display();
	}
	//delete btn;
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
	return rman.loadFont("arial", "../arial.ttf")
		&& rman.loadTexture("normalBtn", "../normal.png")
		&& rman.loadTexture("clikcedBtn", "../clicked.png");
}

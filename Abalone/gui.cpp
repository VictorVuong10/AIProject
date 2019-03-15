#include "gui.h"

gui* gui::cur = nullptr;

gui::gui() : callback{}, window{ sf::VideoMode(1000, 1000), "SFML works!" }
{
	window.setFramerateLimit(30);
	cur = this;
}

gui::gui(game & game) : callback{}, window{ sf::VideoMode(1000, 1000), "SFML works!" }
{
	window.setFramerateLimit(30);
}


gui::~gui()
{
}

void gui::start()
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

	sf::Texture &normal = rman.getTexture("normalBtn");
	sf::Texture clicked = rman.getTexture("clikcedBtn");

	std::bitset<128U> state{ game::GERMAN_DAISY };

	board b{ 500, 500, 300, state };

	button* btn = new button{ &normal, &clicked, "xD", {200, 200} };
	auto handler = new std::function<void(sf::Event&)>{ [&](sf::Event& e) {
		gui::cur->asyncAwait<int>([] {
			std::this_thread::sleep_for(std::chrono::seconds{ 2 });
			return 1;
		}, [&](int i) {
			std::cout << i << std::endl;
			btn->setState(!btn->getVar());
		});
		std::bitset<128U> state{ game::STANDARD };
		b.setState(state);
	} };
	btn->registerHandler(handler);

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
				btn->click(event);
				b.click(event);
			}
		}
		auto now = std::chrono::system_clock::now();
		time_t nowt = std::chrono::system_clock::to_time_t(now);
		char ts[100];
		ctime_s(ts, 100, &nowt);
		text.setString(ts);

		executeCallback();


		window.clear();
		btn->show(std::ref(window));
		b.show(std::ref(window));
		window.draw(text);
		window.display();
	}
	delete btn;
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

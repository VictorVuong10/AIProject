#pragma once
#include "IShowable.h"
#include "button.h"
#include "gui.h"
#include <vector>
#include <queue>
#include <chrono>
#include <ctime>
#include <thread>
#include <iostream>
#include "logic.h"
#include "game.h"

using namespace std;
sf::Mutex qmutex{};
sf::Mutex strmutex{};
string countStr = "";

class task {
public:
	void run() {
		sf::sleep(sf::seconds(10));
		strmutex.lock();
		countStr += "1";
		strmutex.unlock();
	}
};

queue<task*> mq{};

//void runExecutor() {
//	while (window.isOpen()) {
//		if (!mq.empty()) {
//			qmutex.lock();
//			task* t = mq.front();
//			mq.pop();
//			qmutex.unlock();
//			t->run();
//			delete t;
//
//		}
//	}
//
//}

int main() {
	//gui g{};
	//g.start();

	std::bitset<128U> state{ game::STANDARD_STR };
	auto as = logic::getAllValidMove(state, 1);

	std::cout << as.size() << std::endl;
	for (auto v : as) {
		std::cout << v.first.count << " " << v.first.index << " " << v.first.direction << std::endl;
	}

	return 0;
}
//
//int main1()
//{
//	sf::RenderWindow window{ sf::VideoMode(1000, 1000), "SFML works!" };
//	window.setFramerateLimit(30);
//	vector<sf::CircleShape> circles{};
//	sf::Font font;
//	if (!font.loadFromFile("../arial.ttf")) {
//		return 1;
//	}
//	sf::Text text;
//	text.setFont(font);
//	text.setString("hello");
//	text.setFillColor(sf::Color::Red);
//	text.setCharacterSize(24);
//
//	sf::Text counter;
//	counter.setFont(font);
//	counter.setString("");
//	counter.setFillColor(sf::Color::Red);
//	counter.setCharacterSize(24);
//	counter.setPosition(200, 100);
//
//	sf::Texture normal;
//	if (!normal.loadFromFile("../normal.png")) {
//		return 1;
//	}
//	sf::Texture clicked;
//	if (!clicked.loadFromFile("../clicked.png")) {
//		return 1;
//	}
//	
//	button* btn = new button{ &normal, &clicked, "fuck", {200, 200} };
//
//
//	/*for (int i = 0; i < 100; ++i) {
//		sf::CircleShape shape(100.f);
//		shape.setFillColor(sf::Color::Green);
//		shape.setPosition(rand() % 1920, rand() % 1080);
//		circles.push_back(shape);
//	}*/
//
//	//thread *executor = new thread{ &runExecutor };
//	//btn.show(shapes, std::ref(shapesMtx));
//	while (window.isOpen())
//	{
//		sf::Event event;
//		while (window.pollEvent(event))
//		{
//			if (event.type == sf::Event::Closed)
//				window.close();
//			if (event.type == sf::Event::KeyPressed) {
//				if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
//					/*sf::Lock lock(qmutex);
//					mq.push(new task{});*/
//					thread worker{ [&] {
//						sf::sleep(sf::seconds(10));
//						strmutex.lock();
//						countStr += "1";
//						strmutex.unlock();
//					} };
//					worker.detach();
//				}
//			}
//			if (event.type == sf::Event::MouseButtonPressed) {
//				std::cout << "mouse x: " << event.mouseButton.x << std::endl;
//				std::cout << "mouse y: " << event.mouseButton.y << std::endl;
//
//			}
//		}
//		auto now = std::chrono::system_clock::now();
//		time_t nowt = std::chrono::system_clock::to_time_t(now);
//		char ts[100];
//		ctime_s(ts, 100, &nowt);
//		text.setString(ts);
//
//		strmutex.lock();
//		counter.setString(countStr);
//		strmutex.unlock();
//
//		window.clear();
//		/*for (auto c : circles) {
//
//			window.draw(c);
//		}*/
//		
//		/*for (auto s : shapes) {
//			window.draw(*s);
//		}*/
//		btn->show(std::ref(window));
//		window.draw(counter);
//		window.draw(text);
//		window.display();
//	}
//	//executor->join();
//	//delete executor;
//	delete btn;
//	return 0;
//}


//int realMain() {
//
//	//new window
//
//	//load resource
//
//	//new game
//
//	//while loop render
//	while (window.isOpen()) {
//		sf::Event event;
//		while (window.pollEvent(event)) {
//			//handle event single thread
//
//		}
//		
//		//check callback queue
//		//lock pop callback queue and run
//	
//		window.clear();
//		//game show
//		window.display();
//
//	}
//	return 0;
//}
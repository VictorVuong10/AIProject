#pragma once

#include "IShowable.h"
#include "game.h"
#include "button.h"
#include "board.h"
#include "resourceManager.h"
#include <queue>
#include <mutex>
#include <thread>
#include <functional>
#include <iostream>

class gui
{
private:
	std::queue<std::function<void()>> callback;
public:
	static gui* cur;
	sf::RenderWindow window;
	sf::Event event;
	std::mutex cbMtx;
	game* _game;
	gui();
	explicit gui(game& game);
	~gui();
	void start();
	void safePush(std::function<void()> cb);
	void executeCallback();
	bool loadAllResource();

	template<typename T>
	void asyncAwait(std::function<T()> await, std::function<void(T)> then)
	{
		std::function<void()> asyncFunc = [this, then, await]{
			T res = await();
			auto thenWrapper = [res, then] {
				then(res);
			};
			safePush(thenWrapper);
		};
		std::thread worker{ asyncFunc };
		worker.detach();
	}
};


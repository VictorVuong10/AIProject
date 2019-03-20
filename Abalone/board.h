#pragma once
#include "IShowable.h"
#include "IClickable.h"
#include "slot.h"

class game;
class board : public IShowable, virtual public IClickable
{
public:
	static constexpr unsigned int SLOT_NUMBER = 61;
	static constexpr unsigned int ROW_A_I_NUMBER = 5;
	static constexpr unsigned int ROW_B_H_NUMBER = 6;
	static constexpr unsigned int ROW_C_G_NUMBER = 7;
	static constexpr unsigned int ROW_D_F_NUMBER = 8;
	static constexpr unsigned int ROW_E_NUMBER = 9;
	static constexpr unsigned int ROW_NUMBER_MAP[] =
	{ ROW_A_I_NUMBER, ROW_B_H_NUMBER ,
	ROW_C_G_NUMBER , ROW_D_F_NUMBER ,
	ROW_E_NUMBER , ROW_D_F_NUMBER ,
	ROW_C_G_NUMBER , ROW_B_H_NUMBER ,
	ROW_A_I_NUMBER };

	board() = default;
	board(float x, float y, float size, game* game, std::bitset<128U> boardState = std::bitset<128U>(0));
	~board();
	bool checkClick(sf::Event & e) override;
	void click(sf::Event & e) override;
	void show(sf::RenderWindow & window) override;
	void setState(std::bitset<128U> boardState);
	void unSelectAll();
private:
	std::bitset<128U> boardState;
	sf::VertexArray bg;
	std::vector<slot*> slots;
	game* _game;
	void initAllSlots(float x, float y, float size);
};


#pragma once

#include <SFML/Graphics.hpp>

#include "defs.hpp"
#include "VDP.hpp"

class Graphics {
public:
	Graphics(VDP * v);
	~Graphics();
	void test();
	void draw();

private:
	sf::RenderWindow window;
	sf::Texture texture;
	sf::Sprite sprite;

	static constexpr int width = 340;
	static constexpr int height = 240;

	VDP * vdp;
};
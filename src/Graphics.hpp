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

	unsigned width;
	unsigned height;

	VDP * vdp;
};
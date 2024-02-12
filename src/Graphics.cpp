#include "Graphics.hpp"

#include <iostream>

Graphics::Graphics(VDP * v) : vdp(v) {
    width = vdp->getWidth();
    height = vdp->getHeight();
    window.create(sf::VideoMode(width, height), "Megan");
    texture.create(width, height);
}

void Graphics::update() {
    auto & screen = vdp->getScreen();
    texture.update(screen.data());
    sprite.setTexture(texture);
}

void Graphics::draw() {
    update();

    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
    }

    window.clear();
    window.draw(sprite);
    window.display();
}

Graphics::~Graphics() {
}

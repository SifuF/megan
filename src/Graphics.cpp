#include "Graphics.hpp"

#include <iostream>

Graphics::Graphics(VDP * v) : vdp(v) {
    window.create(sf::VideoMode(width, height), "Megan");
    texture.create(width, height);
}

void Graphics::test() {
    auto & screen = vdp->getScreen();
    texture.update(screen.data());
    sprite.setTexture(texture);
}

void Graphics::draw() {
    //while (window.isOpen())
    //{
        test();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(sprite);
        window.display();
    //}
}

Graphics::~Graphics() {
}
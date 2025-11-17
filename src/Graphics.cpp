#include "Graphics.hpp"

#include <iostream>

Graphics::Graphics(VDP * v) : vdp(v) {
    width = vdp->getWidth();
    height = vdp->getHeight();
    window.create(sf::VideoMode(m_scale * width, m_scale * height), "Megan");
    texture.create(width, height);
}

void Graphics::update() {
    auto & screen = vdp->getScreen();
    texture.update(screen.data());

    const auto windowSize = window.getSize();
    const auto textureSize = texture.getSize();

    const auto scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    const auto scaleY = static_cast<float>(windowSize.y) / textureSize.y;

    sprite.setScale(scaleX, scaleY);
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

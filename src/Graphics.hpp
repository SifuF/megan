#pragma once

#include <SFML/Graphics.hpp>

#include "defs.hpp"

class Graphics {
public:
    void create(uint16_t mainWidth, uint16_t mainHeight, uint16_t tileDataWidth, uint16_t tileDataHeight, uint16_t tileMapWidth, uint16_t tileMapHeight);
    void update(const std::vector<uint8_t>& mainBuffer, const std::vector<uint8_t>& tileDataBuffer, const std::vector<uint8_t>& tileMapBuffer);

private:
    sf::RenderWindow m_mainWindow;
    sf::Texture m_mainTexture;
    sf::Sprite m_mainSprite;
    unsigned m_mainScale = 3;

    sf::RenderWindow m_tileDataWindow;
    sf::Texture m_tileDataTexture;
    sf::Sprite m_tileDataSprite;
    unsigned m_tileDataScale = 2;

    sf::RenderWindow m_tileMapWindow;
    sf::Texture m_tileMapTexture;
    sf::Sprite m_tileMapSprite;
    unsigned m_tileMapScale = 1;
};

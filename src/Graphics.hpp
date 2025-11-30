#pragma once

#include <SFML/Graphics.hpp>

#include "defs.hpp"

class Graphics {
public:
    void create(uint16_t mainWidth, uint16_t mainHeight, uint16_t tileDataWidth, uint16_t tileDataHeight, 
        uint16_t scrollMapWidth, uint16_t scrollMapHeight, uint16_t windowMapWidth, uint16_t windowMapHeight);
    
    void update(const std::vector<uint8_t>& mainBuffer, const std::vector<uint8_t>& tileDataBuffer,
        const std::vector<uint8_t>& scrollMapBuffer, const std::vector<uint8_t>& windowMapBuffer);

private:
    sf::RenderWindow m_mainWindow;
    sf::Texture m_mainTexture;
    sf::Sprite m_mainSprite;
    unsigned m_mainScale = 3;

    sf::RenderWindow m_tileDataWindow;
    sf::Texture m_tileDataTexture;
    sf::Sprite m_tileDataSprite;
    unsigned m_tileDataScale = 2;

    sf::RenderWindow m_scrollMapWindow;
    sf::Texture m_scrollMapTexture;
    sf::Sprite m_scrollMapSprite;
    unsigned m_scrollMapScale = 1;

    sf::RenderWindow m_windowMapWindow;
    sf::Texture m_windowMapTexture;
    sf::Sprite m_windowMapSprite;
    unsigned m_windowMapScale = 1;
};

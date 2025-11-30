#include "Graphics.hpp"

void Graphics::create(uint16_t mainWidth, uint16_t mainHeight, uint16_t tileDataWidth, uint16_t tileDataHeight, uint16_t scrollMapWidth, uint16_t scrollMapHeight, uint16_t windowMapWidth, uint16_t windowMapHeight)
{
    auto createWindowAndTexture = [](sf::RenderWindow& window, sf::Texture& texture, uint16_t width, uint16_t height, uint8_t scale, const char* name, const sf::Vector2i& position)
    {
        window.create(sf::VideoMode(scale * width, scale * height), name);
        window.setPosition(position);
        texture.create(width, height);

        return width * scale;
    };

    constexpr uint16_t startX = 100;
    constexpr uint16_t startY = 100;
    constexpr uint8_t delta = 10;

    const auto mainScaledWidth = createWindowAndTexture(m_mainWindow, m_mainTexture, mainWidth, mainHeight, m_mainScale, "Megan - Sega Mega Drive Emulator by SifuF", {startX, startY});
    const auto tileDataScaledWidth = createWindowAndTexture(m_tileDataWindow, m_tileDataTexture, tileDataWidth, tileDataHeight, m_tileDataScale, "Tiles", { startX + mainScaledWidth + delta, startY});
    const auto scrollMapScaledWidth = createWindowAndTexture(m_scrollMapWindow, m_scrollMapTexture, scrollMapWidth, scrollMapHeight, m_scrollMapScale, "Scroll Maps", { startX + mainScaledWidth + tileDataScaledWidth + 2 * delta, startY });
    const auto windowMapScaledWidth = createWindowAndTexture(m_windowMapWindow, m_windowMapTexture, windowMapWidth, windowMapHeight, m_windowMapScale, "Window Map", { startX + mainScaledWidth + tileDataScaledWidth + scrollMapScaledWidth + 3 * delta, startY });
}

void Graphics::update(const std::vector<uint8_t>& mainBuffer, const std::vector<uint8_t>& tileDataBuffer, const std::vector<uint8_t>& scrollMapBuffer, const std::vector<uint8_t>& windowMapBuffer)
{
    auto updateAndDraw = [](sf::RenderWindow& window, sf::Texture& texture, sf::Sprite& sprite, const std::vector<uint8_t>& data)
    {
        texture.update(data.data());
        const auto windowSize = window.getSize();
        const auto textureSize = texture.getSize();
        const auto scaleX = static_cast<float>(windowSize.x) / textureSize.x;
        const auto scaleY = static_cast<float>(windowSize.y) / textureSize.y;
        sprite.setScale(scaleX, scaleY);
        sprite.setTexture(texture);

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(sprite);
        window.display();
    };

    updateAndDraw(m_mainWindow, m_mainTexture, m_mainSprite, mainBuffer);
    updateAndDraw(m_tileDataWindow, m_tileDataTexture, m_tileDataSprite, tileDataBuffer);
    updateAndDraw(m_scrollMapWindow, m_scrollMapTexture, m_scrollMapSprite, scrollMapBuffer);
    updateAndDraw(m_windowMapWindow, m_windowMapTexture, m_windowMapSprite, windowMapBuffer);
}

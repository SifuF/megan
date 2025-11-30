#pragma once

#include "defs.hpp"

#include <array>
#include <stdexcept>
#include <optional>
#include <vector>

struct VDPFrameBuffer {
    std::vector<uint8_t> data;
    uint16_t width;
    uint16_t height;

    VDPFrameBuffer(uint16_t w, uint16_t h) : width(w), height(h), data(w * h * 4) {}

    std::size_t sizeInTiles() { return (width * height) / 64; } // TODO - cache this
    std::size_t widthInTiles() { return width / 8; } // TODO - cache this

    VDPFrameBuffer(const VDPFrameBuffer& other) = delete;
    VDPFrameBuffer& operator=(const VDPFrameBuffer& other) = delete;
    VDPFrameBuffer(VDPFrameBuffer&& other) = delete;
    VDPFrameBuffer& operator=(VDPFrameBuffer&& other) = delete;
};

class VDP {
public:
    void buildFrame();

    uint16_t readCtrlPort();
    void writeCtrlPort(uint16_t data);
    uint16_t readDataPort();
    void writeDataPort(uint16_t data);

    VDPFrameBuffer& getMainFrameBuffer() { return m_mainBuffer; }
    VDPFrameBuffer& getTileDataFrameBuffer() { return m_tileDataBuffer; }
    VDPFrameBuffer& getScrollMapFrameBuffer() { return m_scrollMapBuffer; }
    VDPFrameBuffer& getWindowMapFrameBuffer() { return m_windowMapBuffer; }

    VDP() = default;
    VDP(const VDP& other) = delete;
    VDP& operator=(const VDP& other) = delete;
    VDP(VDP&& other) = delete;
    VDP& operator=(VDP&& other) = delete;

private:
    uint16_t vram16(uint16_t addr);
    void drawTile(VDPFrameBuffer& frameBuffer, uint16_t bufferIndex, uint16_t tile);
    void drawLine(unsigned line, uint8 * tile, unsigned pallet);
    void drawDebugDisplays();

    bool m_debug = true;

    // memory
    std::array<uint8_t, 0xFFFF> m_vram{}; // 0xFFFF bytes
    std::array<uint16_t, 64> m_cram{}; // 64 9bit words
    std::array<uint16_t, 40> m_vsram{}; // 40 10bit words
    std::array<uint8_t, 24> m_reg; // 24 registers

    // access
    uint16_t m_status{};
    uint16_t m_addressRegister{};
    uint8_t m_codeRegister{};

    // register defined state
    uint16_t m_pattern = 0x0000;
    uint16_t m_scrollA = 0xc000;
    uint16_t m_sprite = 0xd800;
    uint16_t m_scrollB = 0xe000;
    uint16_t m_window = 0xf000;
    uint16_t m_hScroll = 0xfc00;

    uint8_t m_autoIncrement = 2; // TODO - check type

    uint16_t m_screenWidth = 320;
    uint16_t m_screenHeight = 224;
    uint8_t m_planeWidth = 64;
    uint8_t m_planeHeight = 32;
    static constexpr uint32_t m_windowSize = 32; // window width == height

    // internal use
    bool m_expectingSecondWord = false;
    std::optional<uint16_t> m_firstWord{};
    
    // frame buffers
    VDPFrameBuffer m_mainBuffer{ m_screenWidth, m_screenHeight };
    VDPFrameBuffer m_tileDataBuffer{256, 512};
    VDPFrameBuffer m_scrollMapBuffer{ m_planeWidth * 8, m_planeHeight * 2 * 8}; // scrollA&B default = 64x32 tiles
    VDPFrameBuffer m_windowMapBuffer{ m_windowSize * 8, m_windowSize * 8 }; // window always = 32x32 tiles 
};

#pragma once

#include "defs.hpp"

#include <vector>

class VDP {
public:
    VDP();

    void clearScreen();
    void clearVram();
    void setup();
    void buildFrame();

    unsigned getWidth() { return width; }
    unsigned getHeight() { return height; }
    std::vector<uint8> & getScreen() { return screen; }

private:

    void drawTile(unsigned x, unsigned y, unsigned t, unsigned c);

    uint16 pattern = 0x0000;
    uint16 scrollA = 0xc000;
    uint16 sprite = 0xd800;
    uint16 scrollB = 0xe000;
    uint16 window = 0xf000;
    uint16 hScroll = 0xfc00;

    std::vector<uint8> vram; // 0xFFFF bytes
    std::vector<uint16> cram; // 64 9bit words
    std::vector<uint16> vsram; // 40 10bit words

    std::vector<uint16> reg; // 24 registers

    std::vector<uint8> screen;

    static constexpr unsigned width = 320;
    static constexpr unsigned height = 224;
};

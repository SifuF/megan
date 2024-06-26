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

    uint16 getStatus() { return status; }

    void setVramIndex(size_t index) { vramIndex = index; }

    void writeVram(uint8 data) { 
        vram[vramIndex++] = data;
       /*
        uint32 a[] = {
          0x12121210,
          0x10000010,
          0x10000010,
          0x11111110,
          0x10000010,
          0x10000010,
          0x10000010,
          0x00000000
        };

        std::memcpy(vram.data(), a, 32);
        */
    }

    void selectRegister(uint8 data);
    void setRegister(uint8 data);

private:

    void drawTile(unsigned x, unsigned y, unsigned tile, unsigned pallet);
    void drawLine(unsigned line, uint8 * tile, unsigned pallet);

    uint16 pattern = 0x0000;
    uint16 scrollA = 0xc000;
    uint16 sprite = 0xd800;
    uint16 scrollB = 0xe000;
    uint16 window = 0xf000;
    uint16 hScroll = 0xfc00;

    std::vector<uint8> vram; // 0xFFFF bytes
    std::vector<uint16> cram; // 64 9bit words
    std::vector<uint16> vsram; // 40 10bit words

    size_t vramIndex;

    std::vector<uint16> reg; // 24 registers
    uint8 currentRegister = 0;
    uint16 status;

    std::vector<uint8> screen;

    static constexpr unsigned width = 320;
    static constexpr unsigned height = 224;

};

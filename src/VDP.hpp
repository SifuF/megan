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

    void processCtrl(uint32 data) {
        if ((data >> 16) == 0) { // potential register write
            const auto index = static_cast<uint8>(data >> 8) - 0x80;
            const auto value = static_cast<uint8>(data);
            reg[index] = value;
        }
        else if ((data >> 29) == 0) { //VRAM write
            currentVramAddr = data & 0x1FFFF;
        }
    }

    void processData(uint32 data) {
        if ((data >> 29) == 0) { // VRAM write
            vram[currentVramAddr] = static_cast<uint8>(data >> 24);
            vram[currentVramAddr + 1] = static_cast<uint8>((data >> 16) & 0xFF);
            vram[currentVramAddr + 2] = static_cast<uint8>((data >> 8) & 0xFF);
            vram[currentVramAddr + 3] = static_cast<uint8>(data & 0xFF);
            currentVramAddr += 4; // autoincrement
        }
    }

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

    uint32 currentVramAddr = 0x0000;

    std::vector<uint16> reg; // 24 registers
    uint16 status;

    std::vector<uint8> screen;

    static constexpr unsigned width = 320;
    static constexpr unsigned height = 224;

};

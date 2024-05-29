#include "VDP.hpp"
#include <iostream>

VDP::VDP() : vram(0xffff), cram(64), vsram(40), reg(24), status(6) {
    screen.resize(width * height * 4);
}

void VDP::clearVram() {
    for (auto& b : vram) {
        b = 0u;
    }
}

void VDP::clearScreen() {
    for (auto& b : screen) {
        b = 0u;
    }
}

void VDP::setup() {
    uint32 f[] = {
        0x12121210,
        0x10000000,
        0x10000000,
        0x11110000,
        0x10000000,
        0x10000000,
        0x10000000,
        0x00000000
    };

    uint32 x[] = {
        0x11111110,
        0x22222220,
        0x33333330,
        0x44444440,
        0x33333330,
        0x22222220,
        0x11111110,
        0x00000000
    };

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

    *(cram.data() + (0 * 16)) = 0b0000'0000'0000'0000; //set pallet 0
    *(cram.data() + (0 * 16) + 1) = 0b0000'0000'0000'1110;
    *(cram.data() + (0 * 16) + 2) = 0b0000'0000'1110'0000;
    *(cram.data() + (0 * 16) + 3) = 0b0000'1110'0000'0000;
    *(cram.data() + (0 * 16) + 4) = 0b0000'0000'1110'1110;

    *(cram.data() + (1 * 16)) = 0b0000'0000'0000'0000; //set pallet 1
    *(cram.data() + (1 * 16) + 1) = 0b0000'0000'0000'1110;
    *(cram.data() + (1 * 16) + 2) = 0b0000'0000'1110'0000;
    *(cram.data() + (1 * 16) + 3) = 0b0000'1110'1110'1110;
    *(cram.data() + (1 * 16) + 4) = 0b0000'1110'1110'1110;

    std::memcpy(vram.data(), x, 32); //copy tile X into 0th tile
    std::memcpy(vram.data() + 32, f, 32); //copy tile F into 1th tile

    std::memset(vram.data() + scrollA, 0, 64 * 32); //fill Scroll A with tile 0
    //std::memset(vram.data() + scrollB, 1, 64 * 32); //fill Scroll B with tile 1

    //random scroll B
    for (int i = 0; i < 64 * 32; ++i) {
        const auto r = rand() % 2;
        *(vram.data() + scrollB + i) = r;
    }
}

void VDP::drawTile(unsigned x, unsigned y, unsigned tile, unsigned pallet) {
    const bool swap = true;
    for (unsigned j = 0; j < 8; j++) { // Tiles are 8x8 pixels. Each row contains 8 pixels so loop through 8 rows
        for (unsigned i = 0; i < 4; i++) { // Each row is 1 32bit long so loop through 4 bytes
            const unsigned rowIndex = (tile * 32) + 4 * j; // Tiles start at location 0x0000 in VRAM. Each tile is 32 bytes (8x8 pixels x 0.5 bytes per pixel), then 4*j to select row

            const uint8 byte = swap ? vram[rowIndex + (3 - i)] : vram[rowIndex + i];
            uint8 msn = byte >> 4;  // Each md pixel is 1 nibble so we do 2 for each i
            uint8 lsn = byte & 0x0F;

            if (swap) {
                std::swap(msn, lsn);
            }

            // x and y are coordinates of tiles, convert to screen pixel coords.
            const unsigned xScreen = x * 8 * 4; // 8 pixels in tile row, 4 bytes RGBA per pixel
            const unsigned yScreen = y * width * 4; // width * 4 RGBA pixels in screen row

            const unsigned tileStartIndex = (xScreen + yScreen * 8 );
            const unsigned col = i * 4 * 2; // each screen pixel is 4 bytes RGBA, and we fill 2 per i
            const unsigned row = width * 4 * j; // width in pixels * 4 bytes per pixel * jth row

            const unsigned screenIndex = tileStartIndex + col + row;

            auto getRGBA = [this](uint8 pixel, unsigned pallet) -> uint32 {
                uint16 colour = *(cram.data() + (pallet * 16) + pixel);
                uint8 r = ((colour & 0x000Fu) >> 1u) * 255/8;
                uint8 g = ((colour & 0x00F0u) >> 5u) * 255/8;
                uint8 b = ((colour & 0x0F00u) >> 9u) * 255/8;

                uint32 rgbaColour = (r << 24) | (g << 16) | (b << 8) | 0xFF;
                return Endian::swapLong(rgbaColour);
            };

            if (lsn != 0) {
                *(reinterpret_cast<uint32*>(screen.data() + screenIndex)) = getRGBA(lsn, pallet);
            }

            if (msn != 0) {
                *(reinterpret_cast<uint32*>(screen.data() + screenIndex + sizeof(uint32))) = getRGBA(msn, pallet);
            }
        }
    }
}

void VDP::drawLine(unsigned line, uint8 * plane, unsigned pallet) {
    const bool swap = true;
    const int tilesPerLine = width / 8;
    for (int j = 0; j < tilesPerLine; ++j) {
        const auto tileDown = line / 8;
        int indexa = j + tilesPerLine * tileDown;
        uint8* indexb = plane + indexa;
        const auto tile = *indexb;
        const unsigned columnIndex = (tile * 32) + 4 * (line % 8);
        for (unsigned i = 0; i < 4; i++) { // Each row is 1 32bit long so loop through 4 bytes
            const uint8 byte = swap ? vram[columnIndex + (3 - i)] : vram[columnIndex + i];
            uint8 msn = byte >> 4;  // Each md pixel is 1 nibble so we do 2 for each i
            uint8 lsn = byte & 0x0F;

            if (swap) {
                std::swap(msn, lsn);
            }

            const unsigned screenIndex = 8*i + 8*4*j + 4*width * line;

            auto getRGBA = [this](uint8 pixel, unsigned pallet) -> uint32 {
                uint16 colour = *(cram.data() + (pallet * 16) + pixel);
                uint8 r = ((colour & 0x000Fu) >> 1u) * 255 / 8;
                uint8 g = ((colour & 0x00F0u) >> 5u) * 255 / 8;
                uint8 b = ((colour & 0x0F00u) >> 9u) * 255 / 8;

                uint32 rgbaColour = (r << 24) | (g << 16) | (b << 8) | 0xFF;
                return Endian::swapLong(rgbaColour);
            };

            if (lsn != 0) {
                *(reinterpret_cast<uint32*>(screen.data() + screenIndex)) = getRGBA(lsn, pallet);
            }

            if (msn != 0) {
                *(reinterpret_cast<uint32*>(screen.data() + screenIndex + sizeof(uint32))) = getRGBA(msn, pallet);
            }
        }
    }
}

void VDP::buildFrame(){
    clearScreen();
    //static unsigned i = 0;
    //for (int i = 0; i < 40; i++) {
    //    for (int j = 0; j < 28; j++) {
    //        drawTile(i, j, *(vram.data() + scrollA + i), 0);
    //        drawTile(i, j, *(vram.data() + scrollB + i), 1);
    //    }
    //}

    for (int i = 0; i < height; i++) {
        drawLine(i, vram.data() + scrollB, 0);
    }
}

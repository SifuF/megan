#pragma once

#include "defs.hpp"

#include <vector>

enum VDPstate {
    VRAMread,
    VRAMwrite,
    CRAMread,
    CRAMwrite,
    VSRAMread,
    VSRAMwrite,
    Idle,
};

class VDP {
public:
    VDP();

    void clearScreen();
    void clearVram();
    void setupTestData();
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
        const auto getVRAMaddress = [](uint32 data) -> uint16 {
            const uint16 bottom = static_cast<uint16>((data >> 16) & 0b0011'1111'1111'1111);
            const uint16 top = static_cast<uint16>(data << 14);
            return top | bottom;
        };

        const auto getCRAMaddress = [](uint32 data) -> uint16 {
            const uint16 bottom = static_cast<uint16>((data >> 16) & 0b0000'0000'0111'1111);
            return bottom;
        };

        const auto getVSRAMaddress = [](uint32 data) -> uint16 {
            const uint16 bottom = static_cast<uint16>((data >> 16) & 0b0000'0000'0111'1111);
            return bottom;
        };

        const bool cd1 = static_cast<bool>(data >> 31);
        const bool cd0 = static_cast<bool>((data >> 30) & 1);
        const bool cd5 = static_cast<bool>((data >> 7) & 1);
        const bool cd4 = static_cast<bool>((data >> 6) & 1);
        const bool cd3 = static_cast<bool>((data >> 5) & 1);
        const bool cd2 = static_cast<bool>((data >> 4) & 1);

        if (((data >> 16) == 0) && ((data >> 15) == 1)) { // Register write
            const auto index = static_cast<uint8>(data >> 8) - 0x80;
            const auto value = static_cast<uint8>(data);
            reg[index] = value;
        }
        else if (cd0 && !cd1 && !cd2 && !cd3 && !cd4 && !cd5) { // VRAM write
            state = VDPstate::VRAMwrite;
            currentAddr = getVRAMaddress(data);
        }
        else if (!cd0 && !cd1 && !cd2 && !cd3 && !cd4 && !cd5) { // VRAM read
            state = VDPstate::VRAMread;
            currentAddr = getVRAMaddress(data);
        }
        else if (cd0 && cd1 && !cd2 && !cd3 && !cd4 && !cd5) { // CRAM write
            state = VDPstate::CRAMwrite;
            currentAddr = getCRAMaddress(data);
        }
        else if (!cd0 && !cd1 && !cd2 && cd3 && !cd4 && !cd5) { // CRAM read
            state = VDPstate::CRAMread;
            currentAddr = getCRAMaddress(data);
        }
        else if (cd0 && !cd1 && cd2 && !cd3 && !cd4 && !cd5) { // VSRAM write
            state = VDPstate::VSRAMwrite;
            currentAddr = getVSRAMaddress(data);
        }
        else if (!cd0 && !cd1 && cd2 && !cd3 && !cd4 && !cd5) { // VSRAM read
            state = VDPstate::VSRAMread;
            currentAddr = getVSRAMaddress(data);
        }
    }

    template<typename DataType>
    void processData(DataType data) {
        switch (state) {
            case VDPstate::VRAMwrite: { // TODO: word write
                if (sizeof(DataType) == 4) {
                    vram[currentAddr] = static_cast<uint8>(data & 0xFF);
                    vram[currentAddr + 1] = static_cast<uint8>((data >> 8) & 0xFF);
                    vram[currentAddr + 2] = static_cast<uint8>((data >> 16) & 0xFF);
                    vram[currentAddr + 3] = static_cast<uint8>(data >> 24);

                    currentAddr += 4; // autoincrement
                }
                else if (sizeof(DataType) == 2) {
                    vram[currentAddr] = static_cast<uint8>(data & 0xFF);
                    vram[currentAddr + 1] = static_cast<uint8>((data >> 8) & 0xFF);

                    currentAddr += 2; // autoincrement
                }
                break;
            }
            case VDPstate::CRAMwrite: {
                cram[currentAddr/2] = data;

                //currentAddr += 2; // autoincrement
                break;
            }
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

    VDPstate state = VDPstate::Idle;
    uint32 currentAddr = 0x0000;

    std::vector<uint16> reg; // 24 registers
    uint16 status;

    std::vector<uint8> screen;

    static constexpr unsigned width = 320;
    static constexpr unsigned height = 224;

};

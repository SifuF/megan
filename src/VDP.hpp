#pragma once

#include "defs.hpp"

#include <array>
#include <stdexcept>
#include <optional>
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

    unsigned getWidth() { return m_width; }
    unsigned getHeight() { return m_height; }
    std::vector<uint8> & getScreen() { return m_screen; }

    uint16_t readCtrlPort()
    {
        m_expectingSecondWord = false;
        return m_status;
    }

    void writeCtrlPort(uint16_t data)
    {
        /*  All VDP bus accesses are 16 - bit(words).
        
            Byte writes are technically undefined, but can be emulated by masking into the 16-bit word:
              - lower byte: (XXAB) with LDS active
              - upper byte: (ABXX) with UDS active
              - VDP latches only the active byte; it does not wait for the other byte
        */
        if (static_cast<uint8_t>(data >> 14) == 0b10)     // register write  
        {                                                 // |7 6 5|4 3 2 1 0| 7 6 5 4 3 2 1 0|                     
            if (!m_expectingSecondWord)                   // |1 0 ?| RS4-RS0 |      D7-D0     |
            {
                const auto index = static_cast<uint8_t>((data >> 8) & 0b11111);
                const auto value = static_cast<uint8>(data);
                if (index < 24) {
                    m_reg[index] = value;
                }
            }
            m_codeRegister = 0;
        }
        else                                        // address write first word
        {                                           // | 7   6 |5 4 3 2 1 0|7 6 5 4 3 2 1 0|  
            if(!m_expectingSecondWord)              // |CD1 CD2|   A13-A8  |      A7-A0    |
            {
                m_expectingSecondWord = true;       // address write second word
                m_firstWord = data;                 // |7 6 5 4 3 2 1 0| 7   6   5   4  3 2  1   0 | 
            }                                       // |0 0 0 0 0 0 0 0|CD5 CD4 CD3 CD2 0 0 CD1 CD0|
            else
            {
                m_expectingSecondWord = false;

                const auto& secondWord = data;
                const auto CD10 = static_cast<uint8_t>(m_firstWord.value() >> 14);
                const auto CD5432 = static_cast<uint8_t>((secondWord >> 2) & 0x1F);
                m_codeRegister = CD5432 | CD10;
                
                // check when these start
                m_dma = static_cast<bool>((secondWord >> 7) & 1); // CD5
                m_vramCopy = static_cast<bool>((secondWord >> 6) & 1); // CD4

                const auto A13toA0 = static_cast<uint16_t>(m_firstWord.value() & 0x3FFF);
                const auto A15A14 = static_cast<uint16_t>(secondWord << 14);
                m_addressRegister = A15A14 | A13toA0;
            }
        }
    }

    uint16_t readDataPort()
    {
        switch (m_codeRegister & 0xF) {
        case 0b0000: { // VRAM read
            const auto msb = m_vram[m_addressRegister];
            const auto lsb = m_vram[m_addressRegister + 1];
            return (msb << 8) | lsb; // TODO - wrap, promotion/conversion, read buffer latency
            break;
        }
        case 0b0001: { // VRAM write
            // do nothing
            break;
        }
        case 0b1000: { // CRAM read
            break;
        }
        case 0b0011: { // CRAM write
            // do nothing
            break;
        }
        case 0b0100: { // VSRAM read
            break;
        }
        case 0b0101: { // VSRAM write
            // do nothing
            break;
        }
        default: {
            throw std::runtime_error("VDP CD3210 bits wrong");
        }
        }
    }

    void writeDataPort(uint16_t data)
    {
        switch (m_codeRegister & 0xF) {
            case 0b0000: { // VRAM read
                // do nothing
                break;
            }
            case 0b0001: { // VRAM write
                uint32 addr = m_addressRegister & 0xFFFE;  // force even, VDP ignores bit 0
                uint32 realAddr = addr & 0x7FFF;           // mask to 32 KB VRAM
                m_vram[realAddr] = static_cast<uint8>(data & 0xFF);
                m_vram[realAddr + 1] = static_cast<uint8>((data >> 8) & 0xFF);
                m_addressRegister = (m_addressRegister + m_autoIncrement) & 0xFFFF;
                break;
            }
            case 0b1000: { // CRAM read
                // do nothing
                break;
            }
            case 0b0011: { // CRAM write
                m_cram[m_addressRegister / 2] = data;
                //currentAddr += 2; // autoincrement
                break;
            }
            case 0b0100: { // VSRAM read
                // do nothing
                break;
            }
            case 0b0101: { // VSRAM write
                break;
            }
            default: {
                throw std::runtime_error("VDP CD3210 bits wrong");
            }
        }
    }

private:

    void drawTile(unsigned x, unsigned y, unsigned tile, unsigned pallet);
    void drawLine(unsigned line, uint8 * tile, unsigned pallet);

    uint16_t m_pattern = 0x0000;
    uint16_t m_scrollA = 0xc000;
    uint16_t m_sprite = 0xd800;
    uint16_t m_scrollB = 0xe000;
    uint16_t m_window = 0xf000;
    uint16_t m_hScroll = 0xfc00;

    bool m_dma = false;
    bool m_vramCopy = false;
    uint8_t m_autoIncrement = 2; // TODO - check type

    std::array<uint8_t, 0xFFFF> m_vram; // 0xFFFF bytes
    std::array<uint16_t, 64> m_cram; // 64 9bit words
    std::array<uint16_t, 40> m_vsram; // 40 10bit words

    size_t m_vramIndex;

    uint16_t m_addressRegister{};
    uint8_t m_codeRegister{};
    uint16_t m_status{};
    std::array<uint16_t, 24> m_reg; // 24 registers
    
    std::vector<uint8> m_screen;

    bool m_expectingSecondWord = false;
    std::optional<uint16_t> m_firstWord{};

    static constexpr unsigned m_width = 320;
    static constexpr unsigned m_height = 224;
};

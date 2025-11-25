#include "VDP.hpp"
#include <iostream>

VDP::VDP()
{
    m_mainBuffer.data.resize(m_mainBuffer.width * m_mainBuffer.height * 4);
    m_tileDataBuffer.data.resize(m_tileDataBuffer.width * m_tileDataBuffer.height * 4);
    m_tileMapBuffer.data.resize(m_tileMapBuffer.width * m_tileMapBuffer.height * 4);
}

uint16_t VDP::readCtrlPort()
{
    m_expectingSecondWord = false;
    return m_status;
}

void VDP::writeCtrlPort(uint16_t data)
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
        if (!m_expectingSecondWord)              // |CD1 CD2|   A13-A8  |      A7-A0    |
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

            const auto A13toA0 = static_cast<uint16_t>(m_firstWord.value() & 0x3FFF);
            const auto A15A14 = static_cast<uint16_t>(secondWord << 14);
            m_addressRegister = A15A14 | A13toA0;
        }
    }
}

uint16_t VDP::readDataPort()
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

void VDP::writeDataPort(uint16_t data)
{
    switch (m_codeRegister & 0xF) {
        case 0b0000: { // VRAM read
            // do nothing
            break;
        }
        case 0b0001: { // VRAM write
            //uint32 addr = m_addressRegister & 0xFFFE;  // force even, VDP ignores bit 0
            //uint32 realAddr = addr & 0x7FFF;           // mask to 32 KB VRAM
            m_vram[m_addressRegister] = static_cast<uint8_t>(data >> 8);
            m_vram[m_addressRegister + 1] = static_cast<uint8_t>(data);
            m_addressRegister = (m_addressRegister + m_autoIncrement) & 0xFFFF;
            break;
        }
        case 0b1000: { // CRAM read
            // do nothing
            break;
        }
        case 0b0011: { // CRAM write
            const auto index = m_addressRegister / 2;
            m_cram[index] = data;
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

void VDP::drawTile(unsigned x, unsigned y, unsigned tile, unsigned pallet)
{
    const bool swap = true;
    for (unsigned j = 0; j < 8; j++) { // Tiles are 8x8 pixels. Each row contains 8 pixels so loop through 8 rows
        for (unsigned i = 0; i < 4; i++) { // Each row is 1 32bit long so loop through 4 bytes
            const unsigned rowIndex = (tile * 32) + 4 * j; // Tiles start at location 0x0000 in VRAM. Each tile is 32 bytes (8x8 pixels x 0.5 bytes per pixel), then 4*j to select row

            const uint8 byte = swap ? m_vram[rowIndex + (3 - i)] : m_vram[rowIndex + i];
            uint8 msn = byte >> 4;  // Each md pixel is 1 nibble so we do 2 for each i
            uint8 lsn = byte & 0x0F;

            if (swap) {
                std::swap(msn, lsn);
            }

            // x and y are coordinates of tiles, convert to screen pixel coords.
            const unsigned xScreen = x * 8 * 4; // 8 pixels in tile row, 4 bytes RGBA per pixel
            const unsigned yScreen = y * m_mainBuffer.width * 4; // width * 4 RGBA pixels in screen row

            const unsigned tileStartIndex = (xScreen + yScreen * 8 );
            const unsigned col = i * 4 * 2; // each screen pixel is 4 bytes RGBA, and we fill 2 per i
            const unsigned row = m_mainBuffer.width * 4 * j; // width in pixels * 4 bytes per pixel * jth row

            const unsigned screenIndex = tileStartIndex + col + row;

            auto getRGBA = [this](uint8 pixel, unsigned pallet) -> uint32 {
                uint16 colour = *(m_cram.data() + (pallet * 16) + pixel);
                uint8 r = ((colour & 0x000Fu) >> 1u) * 255/8;
                uint8 g = ((colour & 0x00F0u) >> 5u) * 255/8;
                uint8 b = ((colour & 0x0F00u) >> 9u) * 255/8;

                uint32 rgbaColour = (r << 24) | (g << 16) | (b << 8) | 0xFF;
                return Endian::swapLong(rgbaColour);
            };

            if (lsn != 0) {
                *(reinterpret_cast<uint32*>(m_mainBuffer.data.data() + screenIndex)) = getRGBA(lsn, pallet);
            }

            if (msn != 0) {
                *(reinterpret_cast<uint32*>(m_mainBuffer.data.data() + screenIndex + sizeof(uint32))) = getRGBA(msn, pallet);
            }
        }
    }
}

void VDP::drawLine(unsigned line, uint8 * plane, unsigned pallet)
{
    const bool swap = true;
    const int tilesPerLine = m_mainBuffer.width / 8;
    for (int j = 0; j < tilesPerLine; ++j) {
        const auto tileDown = line / 8;
        const int indexa = (j + tilesPerLine * tileDown) * sizeof(uint16);
        const uint8* indexb = plane + indexa;
        const auto tile = 2;// *indexb; // TODO - tiles indicies are 16 bit!
        const unsigned columnIndex = (tile * 32) + 4 * (line % 8);
        for (unsigned i = 0; i < 4; i++) { // Each row is 1 32bit long so loop through 4 bytes
            const uint8 byte = swap ? m_vram[columnIndex + (3 - i)] : m_vram[columnIndex + i];
            uint8 msn = byte >> 4;  // Each md pixel is 1 nibble so we do 2 for each i
            uint8 lsn = byte & 0x0F;

            if (swap) {
                std::swap(msn, lsn);
            }

            const unsigned screenIndex = 8*i + 8*4*j + 4* m_mainBuffer.width * line;

            auto getRGBA = [this](uint8 pixel, unsigned pallet) -> uint32 {
                uint16 colour = *(m_cram.data() + (pallet * 16) + pixel);
                uint8 r = ((colour & 0x000Fu) >> 1u) * 255 / 8;
                uint8 g = ((colour & 0x00F0u) >> 5u) * 255 / 8;
                uint8 b = ((colour & 0x0F00u) >> 9u) * 255 / 8;

                uint32 rgbaColour = (r << 24) | (g << 16) | (b << 8) | 0xFF;
                return Endian::swapLong(rgbaColour);
            };

            if (lsn != 0) {
                *(reinterpret_cast<uint32*>(m_mainBuffer.data.data() + screenIndex)) = getRGBA(lsn, pallet);
            }

            if (msn != 0) {
                *(reinterpret_cast<uint32*>(m_mainBuffer.data.data() + screenIndex + sizeof(uint32))) = getRGBA(msn, pallet);
            }
        }
    }
}

void VDP::buildFrame()
{
    //for (int i = 0; i < 40; i++) {
    //    for (int j = 0; j < 28; j++) {
    //        drawTile(i, j, *(m_vram.data() + m_scrollA + i), 0);
    //        drawTile(i, j, *(m_vram.data() + m_scrollB + i), 1);
    //    }
    //}

    for (int i = 0; i < m_mainBuffer.height; i++)
    {
        //drawLine(i, m_vram.data() + m_scrollB, 0);
        drawLine(i, m_vram.data() + m_scrollA, 0);
    }

    static bool once = false;
    if (!once) {
        for (int i = 0; i < m_tileDataBuffer.width * 4 * m_tileDataBuffer.height; i += 4) {
            m_tileDataBuffer.data[i] = 255;
            m_tileDataBuffer.data[i + 1] = 0;
            m_tileDataBuffer.data[i + 2] = 0;
            m_tileDataBuffer.data[i + 3] = 255;
        }

        for (int i = 0; i < m_tileMapBuffer.width * 4 * m_tileMapBuffer.height; i += 4) {
            m_tileMapBuffer.data[i] = 0;
            m_tileMapBuffer.data[i + 1] = 255;
            m_tileMapBuffer.data[i + 2] = 0;
            m_tileMapBuffer.data[i + 3] = 255;
        }
        once = true;
    }
}

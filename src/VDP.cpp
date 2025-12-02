#include "VDP.hpp"
#include <iostream>

uint16_t VDP::readCtrlPort()
{
    m_expectingSecondWord = false;
    return m_status;
}

void VDP::updateRegister(uint8_t index, uint8_t value)
{
    if (index > 23) {
        return;
    }

    m_reg[index] = value;

    switch (index) {
        case 0: {
            break;
        }
        case 1: {
            break;
        }
        case 2: { // scroll A pattern name table
            m_scrollA = static_cast<uint16_t>(value) << 10;
            break;
        }
        case 3: { // window pattern name table
            m_window = static_cast<uint16_t>(value) << 10;
            break;
        }
        case 4: { // scroll B pattern name table
            m_scrollB = static_cast<uint16_t>(value) << 13;
            break;
        }
        case 5: { // sprite attribute name table
            m_sprite = static_cast<uint16_t>(value) << 9;
            break;
        }
        case 6: {
            break;
        }
        case 7: {
            break;
        }
        case 8: {
            break;
        }
        case 9: {
            break;
        }
        case 10: {
            break;
        }
        case 11: {
            break;
        }
        case 12: {
            break;
        }
        case 13: { // h scroll B data table
            m_hScroll = static_cast<uint16_t>(value) << 10; // TODO - check this
            break;
        }
        case 14: {
            break;
        }
        case 15: { // auto increment data
            m_autoIncrement = value;
            break;
        }
        case 16: {
            break;
        }
        case 17: {
            break;
        }
        case 18: {
            break;
        }
        case 19: {
            break;
        }
        case 20: {
            break;
        }
        case 21: {
            break;
        }
        case 22: {
            break;
        }
        case 23: {
            break;
        }
        default: {
            throw std::runtime_error("Invalid VDP register selected!");
            break;
        }
    }
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
            const auto value = static_cast<uint8_t>(data);
            updateRegister(index, value);
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

uint16_t VDP::vram16(uint16_t addr) {
    return static_cast<uint16_t>((m_vram[addr] << 8) | m_vram[addr + 1]);
};

void VDP::drawDebugDisplays()
{
    m_scrollMapBuffer.clear();
    m_windowMapBuffer.clear();
    m_tileDataBuffer.clear();

    for (int i = 0; i < m_tileDataBuffer.sizeInTiles(); i++)
    {
        drawTile(m_tileDataBuffer, i, i);
    }

    const auto planeArea = m_planeWidth * m_planeHeight;
    for (int i = 0; i < planeArea; i++)
    {
        drawTile(m_scrollMapBuffer, i, vram16(m_scrollA + 2 * i));
        drawTile(m_scrollMapBuffer, planeArea + i, vram16(m_scrollB + 2 * i));
    }

    for (int i = 0; i < m_windowSize * m_windowSize; i++)
    {
        drawTile(m_windowMapBuffer, i, vram16(m_window + 2 * i));
    }

    m_horizontalScrollA++;
    //m_verticalScrollA++;
    //m_horizontalScrollB--;
    m_verticalScrollB--;

    auto drawPlaneBox = [this](uint32_t horizontalScroll, uint32_t verticalScroll, uint32_t scrollPlaneSelector)
    {
        auto drawLinePixel = [this](uint32_t index)
        {
            m_scrollMapBuffer.data[index] = 0;
            m_scrollMapBuffer.data[index + 1] = 255;
            m_scrollMapBuffer.data[index + 2] = 0;
            m_scrollMapBuffer.data[index + 3] = 255;
        };

        for (int i = 0; i < m_mainBuffer.width; ++i) {
            const uint32_t x = (horizontalScroll + i) % (m_planeWidth * 8);
            const uint32_t y = (verticalScroll % (m_planeHeight * 8)) + scrollPlaneSelector;
            const uint32_t offset = (x + y * (m_planeWidth * 8)) * 4;
            drawLinePixel(offset);

            const uint32_t yBottom = (verticalScroll + m_screenHeight - 1) % (m_planeHeight * 8) + scrollPlaneSelector;
            const uint32_t offsetRight = (x + yBottom * (m_planeWidth * 8)) * 4;
            drawLinePixel(offsetRight);
        }

        for (int i = 0; i < m_mainBuffer.height; ++i) {
            const uint32_t x = horizontalScroll % (m_planeWidth * 8);
            const uint32_t y = (i + verticalScroll) % (m_planeHeight * 8) + scrollPlaneSelector;
            const uint32_t offset = (x + y * (m_planeWidth * 8)) * 4;
            drawLinePixel(offset);

            const uint32_t xRight = (horizontalScroll + m_screenWidth - 1) % (m_planeWidth * 8);
            const uint32_t offsetRight = (xRight + y * (m_planeWidth * 8)) * 4;
            drawLinePixel(offsetRight);
        }
    };
    drawPlaneBox(m_horizontalScrollA, m_verticalScrollA, 0);
    drawPlaneBox(m_horizontalScrollB, m_verticalScrollB, m_planeHeight * 8);
}

void VDP::drawPixel(VDPFrameBuffer& frameBuffer, uint32_t index, uint8_t nibble, uint8_t pallet)
{
    if (nibble) {
        const auto colourWord = m_cram[pallet * 16 + nibble];
        frameBuffer.data[index] = ((colourWord >> 1) & 0b111) * 255 / 7;
        frameBuffer.data[index + 1] = ((colourWord >> 5) & 0b111) * 255 / 7;
        frameBuffer.data[index + 2] = ((colourWord >> 9) & 0b111) * 255 / 7;
        frameBuffer.data[index + 3] = 255;
    }
}

void VDP::drawTile(VDPFrameBuffer& frameBuffer, uint16_t dst, uint16_t tile)
{
    const TileInfo info{ tile };
    for (unsigned j = 0; j < 8; j++) { // Tiles are 8x8 pixels. Each row contains 8 pixels so loop through 8 rows
        for (unsigned i = 0; i < 4; i++) { // Each row is 1 32bit long so loop through 4 bytes
            const unsigned rowIndex = (info.tileNumber * 32) + 4 * (info.verticalFlip ? 7 - j : j); // Tiles start at location 0x0000 in VRAM. Each tile is 32 bytes (8x8 pixels x 0.5 bytes per pixel), then 4*j to select row (each j has 4 bytes in it)

            const auto byte = m_vram[rowIndex + (info.horizontalFlip ? 3 - i : i)];
            const uint8_t msn = info.horizontalFlip ? byte & 0x0F : byte >> 4;
            const uint8_t lsn = info.horizontalFlip ? byte >> 4 : byte & 0x0F;

            const auto tileStartX = static_cast<uint8_t>(dst % frameBuffer.widthInTiles());
            const auto tileStartY = static_cast<uint8_t>(dst / frameBuffer.widthInTiles());
            const uint32_t bufferIndex = ((tileStartX * 8 + 2 * i) + (tileStartY * 8 + j) * frameBuffer.width) * 4;

            drawPixel(frameBuffer, bufferIndex, msn, info.pallet); // Each md pixel is 1 nibble so we do 2 for each i
            drawPixel(frameBuffer, bufferIndex + 4, lsn, info.pallet);
        }
    }
}

void VDP::drawLine(unsigned line, uint16_t plane, uint32_t horizontalScroll, uint32_t verticalScroll)
{
    for (int j = 0; j < m_mainBuffer.widthInTiles(); ++j) {
        const auto tileY = ((line + verticalScroll) / 8) % m_planeHeight;
        const auto tileX = ((horizontalScroll + j * 8) / 8) % m_planeWidth;
        const auto index = (tileX + m_planeWidth * tileY);
        const auto tile = vram16(plane + 2 * index);
        const TileInfo info{ tile };
        const unsigned columnIndex = (info.tileNumber * 32) + 4 * ((line + verticalScroll) % 8);
        for (unsigned i = 0; i < 4; i++) { // Each row is 1 32bit long so loop through 4 bytes
            const auto byte = m_vram[columnIndex + (info.horizontalFlip ? 3 - i : i)];
            const uint8_t msn = info.horizontalFlip ? byte & 0x0F : byte >> 4;
            const uint8_t lsn = info.horizontalFlip ? byte >> 4 : byte & 0x0F;

            const uint32_t bufferIndex = (2 * i + 8 * j + m_mainBuffer.width * line) * 4;
            drawPixel(m_mainBuffer, bufferIndex, msn, info.pallet);
            drawPixel(m_mainBuffer, bufferIndex + 4, lsn, info.pallet);
        }
    }
}

void VDP::buildFrame()
{
    m_mainBuffer.clear();

    if (m_debug) {
        drawDebugDisplays();
    }
    
    for (int line = 0; line < m_mainBuffer.height; line++)
    {
        drawLine(line, m_scrollB, m_horizontalScrollB, m_verticalScrollB);
        drawLine(line, m_scrollA, m_horizontalScrollA, m_verticalScrollA);
    }
}

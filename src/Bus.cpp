#include "Bus.hpp"

#include <fstream>
#include <iterator>
#include <sstream>
#include <vector>

Bus::Bus() : cpu68000(this)
{
    std::ifstream input("../roms/main.bin", std::ios::binary | std::ios::ate);
    const auto end = input.tellg();
    input.seekg(0, std::ios_base::beg);
    m_map = std::make_unique<uint8[]>(0x1000000);
    input.read((char*)m_map.get(), end);

    printMemory(0x00, 0x100);
    printMemory(0, 0xffff, 0xFF0000);
    printHeader();

    cpu68000.reset();

    graphics.create(vdp.getMainFrameBuffer().width, vdp.getMainFrameBuffer().height,
        vdp.getTileDataFrameBuffer().width, vdp.getTileDataFrameBuffer().height,
        vdp.getTileMapFrameBuffer().width, vdp.getTileMapFrameBuffer().height);

    loop();
}

void Bus::loop() {
    unsigned long long counter = 0;
    while (true) {
        counter++;
        cpu68000.fetchAndDecode();
        if (counter % 500 == 0) {
            vdp.buildFrame();
            graphics.update(vdp.getMainFrameBuffer().data, vdp.getTileDataFrameBuffer().data, vdp.getTileMapFrameBuffer().data);
        }
    }
}

void Bus::printHeader()
{
    uint32_t start = 0x100;
    uint32_t index = start;

    const auto fillHeaderProperty = [&, this](const std::string& name, int length, std::stringstream & stream) {
        stream << name << "=";
        for (; index < start + length; index++)
        {
            if (length <= 4)
            {
                // do nothing
            }
            else
            {
                stream << read8(index);
            }
        }
        stream << '\n';
        start = index;
    };
    
    std::stringstream stream;
    stream << std::hex;

    fillHeaderProperty("Console name", 16, stream);
    fillHeaderProperty("Copyright information", 16, stream);
    fillHeaderProperty("Domestic name", 48, stream);
    fillHeaderProperty("Overseas name", 48, stream);
    fillHeaderProperty("Serial number", 14, stream);
    fillHeaderProperty("Checksum", 2, stream);
    fillHeaderProperty("I/O Support", 16, stream);
    fillHeaderProperty("ROM start address", 4, stream);
    fillHeaderProperty("ROM end address", 4, stream);
    fillHeaderProperty("Backup RAM start", 4, stream);
    fillHeaderProperty("Backup RAM end", 4, stream);
    fillHeaderProperty("Backup RAM support", 12, stream);
    fillHeaderProperty("Modem support", 12, stream);
    fillHeaderProperty("Memo", 40, stream);
    fillHeaderProperty("Country support", 16, stream);

    std::cout << stream.str() << std::endl;
}

void Bus::printMemory(uint32_t begin, uint32_t end, uint32_t offset)
{
    std::stringstream stream;
    stream << std::hex;
    for (unsigned i = begin + offset; i < end + offset; i++)
    {
        if (!(i % 16))
            stream << '\n' << "Offset " << i << ": ";

        if (m_map[i] < 16)
            stream << 0;

        stream << static_cast<int>(m_map[i]) << " ";
    }
    stream << '\n';
    std::cout << stream.str() << std::endl;
}

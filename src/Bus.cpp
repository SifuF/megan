#include "Bus.hpp"

#include <fstream>
#include <iterator>
#include <sstream>
#include <vector>

Bus::Bus() : cpu68000(this), vdp(), graphics(&vdp), hasTmss(true) {
    std::ifstream input("../roms/main.bin", std::ios::binary | std::ios::ate);
    const auto end = input.tellg();
    input.seekg(0, std::ios_base::beg);
    map = std::make_unique<uint8[]>(0x1000000);
    input.read((char*)map.get(), end);

    //printMemory(BusItem::Rom, 0x00, 0x100);
    //printMemory(BusItem::Ram, 0, 0xffff);

    printHeader();
    cpu68000.reset();
    //vdp.setupTestData(); // debug
    loop();
}

Bus::~Bus() {
}

void Bus::loop() {
    while (true) {
        cpu68000.fetchAndDecode();
        vdp.buildFrame();
        graphics.draw();
    }
}

void Bus::printHeader() {
    const auto fillHeaderProperty = [&, this](const std::string& name, int length, std::stringstream & stream) {
        static int start = 0x100;
        static unsigned i = start;
        stream << name << "=";
        for (; i < start + length; i++) {
            if (length <= 4) {
                stream << (int)read<uint8>(i);
            }
            else {
                stream << read<uint8>(i);
            }
        }
        stream << '\n';
        start = i;
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

void Bus::clearMemory(BusItem busItem) {
    switch (busItem) {
        case(BusItem::Rom): {
            std::memset(map.get(), 0, 0x400000);
            break;
        }
        case(BusItem::Ram): {
            std::memset(map.get() + 0xFF0000, 0, 0xFFFF);
            break;
        }
        default: {
            break;
        }
    }
}

void Bus::printMemory(BusItem busItem, unsigned begin, unsigned end) {
    const auto buildString = [this, begin, end](uint32 offset) -> std::string {
        std::stringstream stream;
        stream << std::hex;
        for (unsigned i = begin + offset; i < end + offset; i++) {
            if (!(i % 16))
                stream << '\n' << "Offset " << i << ": ";

            if (map[i] < 16)
                stream << 0;

            stream << static_cast<int>(map[i]) << " ";
        }
        stream << '\n';
        return stream.str();
    };

    switch (busItem) {
        case(BusItem::Rom): {
            std::cout << buildString(0x00) << std::endl;
            break;
        }
        case(BusItem::Ram): {
            std::cout << buildString(0xFF0000) << std::endl;
            break;
        }
        default: {
            break;
        }
    }
}

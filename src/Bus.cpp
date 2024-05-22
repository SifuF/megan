#include "Bus.hpp"

#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

Bus::Bus() : cpu68000(this), vdp(), graphics(&vdp) {
    std::ifstream input("../roms/main.bin", std::ios::binary);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
    map = std::move(buffer);
    map.resize(0x1000000);

    //printMemory(BusItem::Rom, 0x100, 0x200);
    //printMemory(BusItem::Ram, 0, 0xffff);
    printHeader();
    cpu68000.reset();
    //cpu68000.fetch();

    loop();
}

Bus::~Bus() {
}

void Bus::loop() {
    vdp.setup();
    while (true) {
        cpu68000.fetch();
        vdp.buildFrame();
        graphics.draw();
    }

    //while (true);
}

void Bus::printHeader() {
    unsigned s = 0x100;
    unsigned i = s;
    std::ios_base::fmtflags f(std::cout.flags());
    std::cout << std::hex;

    std::cout << "Console name=";
    for (; i < s + 16; i++) {
        std::cout  << readByte(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout << "Copyright information=";
    for (; i < s+16; i++) {
        std::cout << readByte(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout << "Domestic name=";
    for (; i < s + 48; i++) {
        std::cout << readByte(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout << "Overseas name=";
    for (; i < s + 48; i++) {
        std::cout << readByte(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout << "Serial number=";
    for (; i < s + 14; i++) {
        std::cout << readByte(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout << "Checksum=";
    std::cout << readWord(i);
    std::cout << std::endl;
    i += 2;
    s = i;

    std::cout << "I/O Support=";
    for (; i < s + 16; i++) {
        std::cout << readByte(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout << "ROM start address=";
    std::cout << readLong(i);
    std::cout << std::endl;
    i += 4;
    s = i;

    std::cout << "ROM end address=";
    std::cout << readLong(i);
    std::cout << std::endl;
    i += 4;
    s = i;

    std::cout << "Backup RAM start=";
    std::cout << readLong(i);
    std::cout << std::endl;
    i += 4;
    s = i;

    std::cout << "Backup RAM end=";
    std::cout << readLong(i);
    std::cout << std::endl;
    i += 4;
    s = i;

    std::cout << "Backup RAM support=";
    for (; i < s + 12; i++) {
        std::cout << readByte(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout << "Modem support=";
    for (; i < s + 12; i++) {
        std::cout << readByte(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout << "Memo=";
    for (; i < s + 40; i++) {
        std::cout << readByte(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout << "Country support=";
    for (; i < s + 16; i++) {
        std::cout << readByte(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout.flags(f);
}

void Bus::clearMemory(BusItem busItem) {
    switch (busItem) {
        case(BusItem::Rom): {
            std::memset(map.data(), 0, 0x400000);
            break;
        }
        case(BusItem::Ram): {
            std::memset(map.data() + 0xFF0000, 0, 0xFFFF);
            break;
        }
        default: {
            break;
        }
    }
}

void Bus::printMemory(BusItem busItem, unsigned begin, unsigned end) {
    switch (busItem) {
        case(BusItem::Rom): {
            for (unsigned i = begin; i < end; i++) {
                if (!(i % 16))
                    std::cout << std::endl << "offset " << i << ": ";

                if (map.at(i) < 16)
                    std::cout << 0;

                std::cout << static_cast<unsigned char>(map.at(i)) << " ";
            }
            std::cout << std::endl;
            break;
        }

        case(BusItem::Ram): {
            for (unsigned i = begin + 0xFF0000; i < end + 0xFF0000; i++) {
                if (!(i % 16))
                    std::cout << std::endl << "offset " << std::hex << i << ": ";

                if (map.at(i) < 16)
                    std::cout << 0;

                std::cout << static_cast<unsigned>(map.at(i)) << " ";
            }
            std::cout << std::endl;
            break;
        }
        default: {
            break;
        }
    }
}

uint8 Bus::readByte(uint32 addr) {
    if (addr < 0x400000) {
        //std::cout << "ROM read";
    }
    else if (addr < 0x800000) {
        std::cout << "Error - Sega CD and 32X space read ";
        return 0;
    }
    else if (addr < 0xA00000) {
        std::cout << "Error - 32X ? space read ";
        return 0;
    }
    else if (addr < 0xA10000) {
        std::cout << "Z80 space read ";
    }
    else if (addr < 0xA10020) {
        std::cout << "IO read ";
        return 0; // No TMSS
    }
    else if (addr < 0xFF0000) {
        std::cout << "Not implemented read yet! ";
        return 0;
    }
    else if (addr < 0x1000000) {
        std::cout << "IO read ";
    }
    else {
        std::cout << "Error - read above address space! ";
        return 0;
    }

    return map[addr];
}

uint16 Bus::readWord(uint32 addr) {
    const uint16 msb = static_cast<uint16>(readByte(addr)) << 8;
    const uint16 lsb = static_cast<uint16>(readByte(addr + 1));
    return msb | lsb;
}

uint32 Bus::readLong(uint32 addr) {
    const uint32 msb = static_cast<uint32>(readWord(addr)) << 16;
    const uint32 lsb = static_cast<uint32>(readWord(addr + 2));
    return msb | lsb;
}

void Bus::writeByte(uint32 addr, uint8 data) {
    if (addr < 0x400000) {
        std::cout << "Error - ROM write";
    }
    else if (addr < 0x800000) {
        std::cout << "Error - Sega CD and 32X space write";
    }
    else if (addr < 0xA00000) {
        std::cout << "Error - 32X ? space write";
    }
    else if (addr < 0xA10000) {
        std::cout << "Z80 space write";
    }
    else if (addr < 0xA10020) {
        std::cout << "IO write";
    }
    else if (addr < 0xFF0000) {
        std::cout << "Not implemented write yet!";
    }
    else if(addr < 0x1000000) {
        std::cout << "RAM write";
    }
    else {
        std::cout << "Error - write above address space!";
    }

    map[addr] = data;
}

void Bus::writeWord(uint32 addr, uint16 data) {
    const uint8 msb = static_cast<uint8>(data);
    const uint8 lsb = static_cast<uint8>(data << 8);
    writeByte(addr, msb);
    writeByte(addr + 1, lsb);
}

void Bus::writeLong(uint32 addr, uint32 data) {
    const uint16 msb = static_cast<uint16>(data);
    const uint16 lsb = static_cast<uint16>(data << 16);
    writeWord(addr, msb);
    writeWord(addr + 2, lsb);
}

#include "Bus.hpp"

#include <fstream>
#include <iterator>
#include <vector>

Bus::Bus() : cpu68000(this), vdp(), graphics(&vdp), hasTmss(true) {
    std::ifstream input("../roms/main.bin", std::ios::binary);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
    map = std::make_unique<uint8[]>(0x1000000);
    std::memcpy(map.get(), buffer.data(), buffer.size());

    //printMemory(BusItem::Rom, 0x100, 0x200);
    //printMemory(BusItem::Ram, 0, 0xffff);

    printHeader();
    cpu68000.reset();
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
}

void Bus::printHeader() {
    unsigned s = 0x100;
    unsigned i = s;
    std::ios_base::fmtflags f(std::cout.flags());
    std::cout << std::hex;

    std::cout << "Console name=";
    for (; i < s + 16; i++) {
        std::cout  << read<uint8>(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout << "Copyright information=";
    for (; i < s+16; i++) {
        std::cout << read<uint8>(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout << "Domestic name=";
    for (; i < s + 48; i++) {
        std::cout << read<uint8>(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout << "Overseas name=";
    for (; i < s + 48; i++) {
        std::cout << read<uint8>(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout << "Serial number=";
    for (; i < s + 14; i++) {
        std::cout << read<uint8>(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout << "Checksum=";
    std::cout << read<uint16>(i);
    std::cout << std::endl;
    i += 2;
    s = i;

    std::cout << "I/O Support=";
    for (; i < s + 16; i++) {
        std::cout << read<uint8>(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout << "ROM start address=";
    std::cout << read<uint32>(i);
    std::cout << std::endl;
    i += 4;
    s = i;

    std::cout << "ROM end address=";
    std::cout << read<uint32>(i);
    std::cout << std::endl;
    i += 4;
    s = i;

    std::cout << "Backup RAM start=";
    std::cout << read<uint32>(i);
    std::cout << std::endl;
    i += 4;
    s = i;

    std::cout << "Backup RAM end=";
    std::cout << read<uint32>(i);
    std::cout << std::endl;
    i += 4;
    s = i;

    std::cout << "Backup RAM support=";
    for (; i < s + 12; i++) {
        std::cout << read<uint8>(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout << "Modem support=";
    for (; i < s + 12; i++) {
        std::cout << read<uint8>(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout << "Memo=";
    for (; i < s + 40; i++) {
        std::cout << read<uint8>(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout << "Country support=";
    for (; i < s + 16; i++) {
        std::cout << read<uint8>(i);
    }
    std::cout << std::endl;
    s = i;

    std::cout.flags(f);
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
    switch (busItem) {
        case(BusItem::Rom): {
            for (unsigned i = begin; i < end; i++) {
                if (!(i % 16))
                    std::cout << std::endl << "offset " << i << ": ";

                if (map[i] < 16)
                    std::cout << 0;

                std::cout << static_cast<unsigned char>(map[i]) << " ";
            }
            std::cout << std::endl;
            break;
        }

        case(BusItem::Ram): {
            for (unsigned i = begin + 0xFF0000; i < end + 0xFF0000; i++) {
                if (!(i % 16))
                    std::cout << std::endl << "offset " << std::hex << i << ": ";

                if (map[i] < 16)
                    std::cout << 0;

                std::cout << static_cast<unsigned>(map[i]) << " ";
            }
            std::cout << std::endl;
            break;
        }
        default: {
            break;
        }
    }
}


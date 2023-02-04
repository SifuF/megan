#include "Bus.hpp"

#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

Bus::Bus() {
	cpu68000 = new Cpu68000(this);

	std::ifstream input("roms/sonic.bin", std::ios::binary);
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
	rom = std::move(buffer);
	ram.resize(0xffff);

	//printMemory(BusItem::Rom, 0x100, 0x200);
	//printMemory(BusItem::Ram, 0, 0xffff);
	printHeader();
	cpu68000->reset();
	cpu68000->fetch();
}

Bus::~Bus() {
	delete cpu68000;
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
			std::memset(rom.data(), 0, rom.size());
			break;
		}

		case(BusItem::Ram): {
			std::memset(ram.data(), 0, ram.size());
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

				if (rom.at(i) < 16)
					std::cout << 0;

				std::cout << static_cast<unsigned char>(rom.at(i)) << " ";
			}
			std::cout << std::endl;
			break;
		}

		case(BusItem::Ram): {
			for (unsigned i = begin; i < end; i++) {
				if (!(i % 16))
					std::cout << std::endl << "offset " << std::hex << i << ": ";

				if (ram.at(i) < 16)
					std::cout << 0;

				std::cout << static_cast<unsigned>(ram.at(i)) << " ";
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
		return rom[addr];
	}
	else if (addr < 0xA00000) {
		std::cout << "Error - 32X space read";
		return 0;
	}
	else if (addr < 0xFF0000) {
		std::cout << "Not implemented read yet!";
		return 0;
	}
	else if (addr < 0x1000000) {
		// return ram
		return 0;
	}
	else {
		std::cout << "Error - read above address space!";
		return 0;
	}
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


void Bus::writeLong(uint32 addr, uint32 data) {
	if (addr < 0x400000) {
		std::cout << "Error - ROM write";
	}
	else if (addr < 0xA00000) {
		std::cout << "Error - 32X space write";
	}
	else if (addr < 0xFF0000) {
		std::cout << "Not implemented write yet!";
	}
	else if(addr < 0x1000000) {
		// write ram
	}
	else {
		std::cout << "Error - write above address space!";
	}
}
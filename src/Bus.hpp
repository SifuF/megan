#pragma once

#include "Cpu68000.hpp"

#include <vector>

enum class BusItem {
	Rom,
	Ram
};

class Bus {
public:
	Bus();
	~Bus();

	void clearMemory(BusItem busItem);
	void printMemory(BusItem busItem, unsigned begin, unsigned end);
	void printHeader();

	uint8 readByte(uint32 addr);
	uint16 readWord(uint32 addr);
	uint32 readLong(uint32 addr);

	void writeByte(uint32 addr, uint8 data);
	void writeWord(uint32 addr, uint16 data);
	void writeLong(uint32 addr, uint32 data);

private:
	Cpu68000* cpu68000;

	std::vector<uint8> rom; //0x400000 bytes
	std::vector<uint8> ram; //0xFFFF bytes

};

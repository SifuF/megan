#include "VDP.hpp"
#include<iostream>


VDP::VDP() {
	vram.resize(0xffff);
	cram.resize(64);
	vsram.resize(40);
	reg.resize(24);
	screen.resize(340 * 240 * 4);
}

void VDP::clearVram() {
	for (auto& b : vram) {
		b = 0u;
	}
}

void VDP::setup() {
	//copy tile X into 0th tile
	uint32 x[] = {
	    0x10000010,
	    0x01000100,
	    0x00101000,
	    0x00010000,
	    0x00101000,
	    0x01000100,
	    0x10000010,
	    0x00000000
	};
	//std::memcpy(vram.data(), x, 32);

	//copy tile F into 1th tile
	uint32 f[] = {
		0x11111110,
		0x10000000,
		0x10000000,
		0x11110000,
		0x10000000,
		0x10000000,
		0x10000000,
		0x00000000
	};
	std::memcpy(vram.data(), f, 32);

	//fill Scroll A with tile 0
	//std::memset(vram.data() + scrollA, 0, 64 * 32);

	//fill Scroll B with tile 1
	//std::memset(vram.data() + scrollB, 1, 64 * 32);

	for (int i = 0; i < 8; i++) {
		std::cout << std::hex << (unsigned)f[i] << std::endl;
	}
}

void VDP::buildFrame() {
	/*for (unsigned i = 0; i < 340 * 240 * 4; i += 4) {
		screen[i] = rand() % 256;
		screen[i + 1] = rand() % 256;
		screen[i + 2] = rand() % 256;
		screen[i + 3] = rand() % 256;
	}*/
	bool swap = true;
	unsigned width = 340;
	for (unsigned j = 0; j < 8; j++)
	{
		for (unsigned i = 0; i < 4; i++)
		{
			uint8 byte;
			if (swap) {
				byte = vram[(3 - i) + 4 * j];
			}
			else {
				byte = vram[i + 4 * j];
			}
			uint8 msn = byte >> 4;
			uint8 lsn = byte & 0x0F;

			if (swap) {
				std::swap(msn, lsn);
			}
			screen[i * 8 + width * 4 * j] = lsn * 255;
			screen[i * 8 + 1 + width * 4 * j] = 0;
			screen[i * 8 + 2 + width * 4 * j] = 0;
			screen[i * 8 + 3 + width * 4 * j] = 255;

			screen[i * 8 + 4 + width * 4 * j] = msn * 255;
			screen[i * 8 + 5 + width * 4 * j] = 0;
			screen[i * 8 + 6 + width * 4 * j] = 0;
			screen[i * 8 + 7 + width * 4 * j] = 255;
		}
	}
}
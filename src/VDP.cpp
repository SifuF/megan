#include "VDP.hpp"
#include<iostream>

VDP::VDP() {
	vram.resize(0xffff);
	cram.resize(64);
	vsram.resize(40);
	reg.resize(24);
	screen.resize(width * height * 4);
}

void VDP::clearVram() {
	for (auto& b : vram) {
		b = 0u;
	}
}

void VDP::clearScreen() {
	for (auto& b : screen) {
		b = 0u;
	}
}

void VDP::setup() {

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

	//copy tile X into 0th tile
	std::memcpy(vram.data(), x, 32);

	//copy tile F into 1th tile
	std::memcpy(vram.data() + 32, f, 32);

	//fill Scroll A with tile 0
	std::memset(vram.data() + scrollA, 0, 64 * 32);

	//fill Scroll B with tile 1
	std::memset(vram.data() + scrollB, 1, 64 * 32);

	for (int i = 0; i < 8; i++) {
		std::cout << std::hex << (unsigned)f[i] << std::endl;
	}
	std::cout << std::endl;

	uint16 test = 0xF12a;
	uint16 swapped = Endian::swapWord(test);
	std::cout << std::hex << test << std::endl;
	std::cout << std::hex << swapped << std::endl;

	uint32 test32 = 0xF12a65de;
	uint32 swapped32 = Endian::swapLong(test32);
	std::cout << std::hex << test32 << std::endl;
	std::cout << std::hex << swapped32 << std::endl;
}

void VDP::fillRand() {
	for (unsigned i = 0; i < width * height * 4; i += 4) {
		screen[i] = rand() % 256;
		screen[i + 1] = rand() % 256;
		screen[i + 2] = rand() % 256;
		screen[i + 3] = rand() % 256;
	}
}

void VDP::drawTile(unsigned x, unsigned y, unsigned t, unsigned c) {
	const bool swap = true;
	for (unsigned j = 0; j < 8; j++) { // tiles are 8x8 pixels so loop through 8 rows
		for (unsigned i = 0; i < 4; i++) { // each row is 1 long so loop through 4 bytes
			uint8 byte;
			const unsigned v = (t * 32) + 4 * j; // each tile is 32 bytes (8x8 pixels x 0.5 bytes per pixel) then 4*j to select row
			if (swap) {
				byte = vram[v + (3 - i)];
			}
			else {
				byte = vram[v + i];
			}
			uint8 msn = byte >> 4;  // each md pixel is 1 nibble so we do 2 for each i
			uint8 lsn = byte & 0x0F;

			if (swap) {
				std::swap(msn, lsn);
			}

			unsigned pos = x*32 + y* width*4*8;
			const unsigned col = i * 8; // each screen pixel is 4 bytes RGBA. We fill 2 per i so *8
			const unsigned row = width * 4 * j; // width in pixels * 4 bytes per pixel * jth row

			const unsigned s = pos + col + row;

			unsigned r = 0;
			unsigned g = 0;
			unsigned b = 0;

			if (c == 0) {
				r = 255;
				g = 0;
				b = 0;
			}
			else if (c == 1) {
				r = 0;
				g = 255;
				b = 0;
			}

			screen[s] |= lsn * r;
			screen[s + 1] |= lsn * g;
			screen[s + 2] |= lsn * b;
			screen[s + 3] |= 255;

			screen[s + 4] |= msn * r;
			screen[s + 5] |= msn * g;
			screen[s + 6] |= msn * b;
			screen[s + 7] |= 255;
		}
	}
}

void VDP::buildFrame(){
	clearScreen();
	static unsigned i = 0;
	for (int i = 0; i < 40; i++) {
		for (int j = 0; j < 28; j++) {
			//drawTile(i, j, 0, 0);
			drawTile(i, j, 1, 1);
		}
	}
	//fillRand();
}
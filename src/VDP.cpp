#include "VDP.hpp"

VDP::VDP() {
	vram.resize(0xffff);
	cram.resize(64);
	vsram.resize(40);
	reg.resize(24);
}
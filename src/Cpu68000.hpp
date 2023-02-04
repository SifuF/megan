#pragma once

#include "defs.hpp"
#include <array>
#include <bitset>

#define BYTE 1
#define WORD 2
#define LONG 3

class Bus;

struct Reg {
	union {
		uint32 l;
		uint16 w;
		uint8 b;
	};
};

enum Flag {
	C = 0,
	V,
	Z,
	N,
	X,
	Unused5,
	Unused6,
	Unused7,
	I0,
	I1,
	I2,
	Unused11,
	Unused12,
	S,
	unused14,
	T
};

class Cpu68000 {
public:
	Cpu68000(Bus* b);
	~Cpu68000();

	void reset();

	void fetch();
	void decode();
	void execute();

private:
	void move(uint8 src, uint8 dst, int size) {
	
	
	};
	void andi_b();
	void add_l();
	void add_w();
	void beq();
	void lea();
	void dbra();
	void jmp();




	std::array<Reg, 8> D{};
	std::array<Reg, 8> A{};  //A7 is SP and SSP
	uint32 PC; // 24 bits
	std::bitset<16> SR; // 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
                        //  T     S       I2 I1 I0           X  N  Z  V  C

	Bus* bus;
};

/* 15 to 8 system byte. 7 to 0 user byte.
   T = Trace Bit {0 = normal, 1 = debug}
   S = Supervisor State {0 = user, 1 = supervisor}
   I2-I0 = Interrupt Mask

   X = Extend {Similar to carry flag}
   N = Negative {ALU result is negative}
   Z = Zero {ALU result is zero}
   V = Overflow {ALU result out of range}
   C = Carry {ALU results in carry out of MSB or requires borrow into MSB of operand}
*/
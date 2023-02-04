#include "Cpu68000.hpp"

#include "Bus.hpp"

#include <iostream>

Cpu68000::Cpu68000(Bus* b) : bus(b), PC(0), SR(0) {

}

Cpu68000::~Cpu68000() {}

void Cpu68000::reset() {
	for (int i = 0; i < 8; i++) {
		A[i].l = 0x0;
		D[i].l = 0x0;
	}
	SR = 0x0;
	A[7].l = bus->readLong(0x000000);
	PC = bus->readLong(0x000004);
}


void Cpu68000::fetch() {
		const uint16 instruction = bus->readWord(PC);
		PC += 2;

		const uint8 bits0to3 = static_cast<uint8>(instruction >> 12);
		const uint8 bits0to1 = static_cast<uint8>(instruction >> 14);
		const uint8 bits2to3 = static_cast<uint8>((instruction >> 12) & 0x3u);
		const uint8 bits4to6 = static_cast<uint8>((instruction >> 9) & 0x7u);

		if (bits0to3 == 0) {
			if (bit7 == 0) {
				if (bits4to6 == 0) {
					if (bits8to15 == 0b00111100) {
						ORItoCCR();
					}
					else if (bits8to15 == 0b01111100) {
						ORItoSR();
					}
					else {
						ORI(bits8to9, bits10to12, bits13to15);
					}
				}
				else if (bits4to6 == 1) {
					if (bits8to15 == 0b00111100) {
						ANDItoCCR();
					}
					else if (bits8to15 == 0b01111100) {
						ANDItoSR();
					}
					else {
						ANDI(bits8to9, bits10to12, bits13to15);
					}
				}
				else if (bits4to6 == 2) {
					SUBI(bits8to9, bits10to12, bits13to15);
				}
				else if (bits4to6 == 3) {
					ADDI(bits8to9, bits10to12, bits13to15);
				}
				else if (bits4to6 == 4) {
					if (bits8to9 == 0) {
						BTST(bits10to12, bits13to15);
					}
					else if (bits8to9 == 1) {
						BCHG(bits10to12, bits13to15);
					}
					else if (bits8to9 == 2) {
						BCLR(bits10to12, bits13to15);
					}
					else {
						BSET(bits10to12, bits13to15);
					}
				}
				else if (bits4to6 == 5) {
					if (bits8to15 == 0b00111100) {
						EORItoCCR();
					}
					else if (bits8to15 == 0b01111100) {
						EORItoSR();
					}
					else {
						EORI(bits8to9, bits10to12, bits13to15);
					}
				}
				else if (bits4to6 == 6) {
					CMPI(bits8to9, bits10to12, bits13to15);
				}
				else {
					std::cout << "Error" << std::endl;
				}
			}
			else {
				if (bits10_12 == 1) {
					MOVEP(bits4to6, bit8, bit9, bits13to15);
				}
				else if (bits8_9 == 0) {
					BTST(bits4to6, bits10to12, bits13to15);
				}
				else if (bits8_9 == 1) {
					BCHG(bits4to6, bits10to12, bits13to15);
				}
				else if (bits8_9 == 2) {
					BCLR(bits4to6, bits10to12, bits13to15);
				}
				else {
					BSET(bits4to6, bits10to12, bits13to15);
				}
		    }
		}
		else if (bits0to3 < 4) {
			if (bits7to9 == 1) {
				MOVEA(bits2to3, bits4to6, bits10to12, bits13to15);
			}
			else {
				MOVE(bits2to3, bits4to6, bits7to9, bits10to12, bits13to15);
			}
		}
		else if (bits0to3 == 4) {
			if (bits4to15 == 0b101011111100) {
				ILLEGAL();
			}
			else if (bits4to9 == 0b101011) {
				TAS(bits10to12, bits13to15);
			}
			else if (bits4to7 == 0b1010) {
				TST(bits8to9, bits10to12, bits13to15);
			}
			else if (bits4to7 == 0b11100100) {
				TRAP(bits12to15);
			}
			else if (bits4to12 == 0b111001010) {
				LINK(bits13to15);
			}
			else if (bits4to12 == 0b111001011) {
				ULINK(bits13to15);
			}
			else if (bits4to11 == 0b11100110) {
				MOVEUSP(bit12, bits13to15);
			}
			else if (bits4to15 == 0b111001110000) {
				RESET();
			}
			else if (bits4to15 == 0b111001110001) {
				NOP();
			}
			else if (bits4to15 == 0b111001110010) {
				STOP();
			}
			else if (bits4to15 == 0b111001110011) {
				RTE();
			}
			else if (bits4to15 == 0b111001110101) {
				RTS();
			}
			else if (bits4to15 == 0b111001110110) {
				TRAPV();
			}
			else if (bits4to15 == 0b111001110111) {
				RTR();
			}
			else if (bits4to9 == 0b111010) {
				JSR(bits10to12, bits13to15);
			}
			else if (bits4to15 == 0b111011) {
				JMP(bits10to12, bits13to15);
			}
			else if (bit4 == 1 && bits6to8 == 1) {
				MOVEM(bit5, bit9, bits10to12, bits13to15);
			}
			else if (bits7to9 == 7) {
				LEA(bits4to6, bits10to12, bits13to15);
			}
			else if (bits7to9 == 6) {
				CHK(bits4to6, bits10to12, bits13to15);
			}
			else if (bits4to6 == 4 && bits7to8 == 1 && bits10to12 == 0) {
				EXT(bit9, bits13to15);
			}
			else if (bits4to6 == 4 && bits7to9 == 0) {
				NBCD(bits10to12, bits13to15);
			}
			else if (bits4to6 == 4 && bits7to9 == 1 && bits10to12 == 0) {
				SWAP(bits13to15);
			}
			else if (bits4to6 == 4 && bits7to9 == 1) {
				PEA(bits10to12, bits13to15);
			}
			else if (bits8to9 == 3) {
				if (bits4to7 == 0) {
					MOVEfromSR(bits10to12, bits13to15);
				}
				else if (bits4to7 == 4) {
					MOVEtoCCR(bits10to12, bits13to15);
				}
				else if (bits4to7 == 6) {
					MOVEtoSR(bits10to12, bits13to15);
				}
			}
			else {
				if (bits4to7 == 0) {
					NEGX(bits8to9, bits10to12, bits13to15);
				}
				else if (bits4to7 == 2) {
					CLR(bits8to9, bits10to12, bits13to15);
				}
				else if (bits4to7 == 4) {
					NEG(bits8to9, bits10to12, bits13to15);
				}
				else if (bits4to7 == 6) {
					NOT(bits8to9, bits10to12, bits13to15);
				}
			}
		}
		else if (bits0to3 == 5) {
			if (bits8to9 == 3) {
				if (bits10to12 == 1) {
					DBcc(bits4to7, bits13to15);
				}
				else {
					Scc(bits4to7, bits10to12, bits13to15);
				}
			}
			else {
				if (bit7 == 1) {
					SUBQ(bits4to6, bits8to9, bits10to12, bits13to15);
				}
				else {
					ADDQ(bits4to6, bits8to9, bits10to12, bits13to15);
				}
			}
		}
		else if (bits0to3 == 6) {
			if (bits4to7 == 0) {
				BRA(bits8to15);
			}
			else if (bits4to7 == 1) {
				BSR(bits8to15);
			}
			else {
				Bcc(bits4to7, bits8to15);
			}
		}
		else if (bits0to3 == 7) {
		    MOVEQ(bits4to6, bits8to15);
		}
		else if (bits0to3 == 8) {
			if (bits8to9 == 3) {
				if (bit7 == 1) {
					DIVS(bits4to6, bits8to9, bits10to12, bits13to15);
				}
				else {
					DIVU(bits4to6, bits8to9, bits10to12, bits13to15);
				}
			}
			else if (bits7to11 == 16) {
				SBCD(bits4to6, bit12, bits13to15);
			}
			else {
				OR(bits4to6, bit7, bits8to9, bits10to12, bits13to15);
			}
		}
		else if (bits0to3 == 9) {
			if (bits8to9 == 3) {
				SUBA(bits4to6, bit7, bits10to12, bits13to15);
			}
			else if (bit7 == 1 && bits10to11 == 0) {
				SUBX(bits4to6, bits8to9, bit12, bits13to15);
			}
			else {
				SUB(bits4to6, bit7, bits8to9, bits10to12, bits13to15;
			}
		}
		else if (bits0to3 == 11) {
			if (bits8to9 == 3) {
				CMPA(bits4to6, bit7, bits10to12, bits13to15);
			}
			else if (bits10to12 == 1) {
				CMPM(bits4to6, bits8to9, bits13to15);
			}
			else if (bit7 == 0) {
				CMP(bits4to6, bits8to9, bits10to12, bits13to15);
			}
			else {
				EOR(bits4to6, bits8to9, bits10to12, bits13to15);
			}
		}
		else if (bits0to3 == 12) {
			if (bits7to11 == 16) {
				ABCD(bits4to6, bit12, bits13to15);
			}
			else if (bit7 == 0 && bit8to9 == 3) {
				MULU(bits4to6, bits10to12, bits13to15);
			}
			else if (bit7 == 1 && bit8to9 == 3) {
				MULS(bits4to6, bits10to12, bits13to15);
			}
			else if (bit7 == 1 && bit10to11 == 0) {
				EXG(bits4to6, bits8to9, bit12, bits13to15);
			}
			else {
				AND(bits4to6, bit7, bits8to9, bits10to12, bits13to15);
			}
		}
		else if (bits0to3 == 13) {
			if (bits8to9 == 3) {
				ADDA(bits4to6, bit7, bits10to12, bits13to15);
			}
			else if (bit7 == 1 && bits10to11 == 0) {
				ADDX(bits4to6, bits8to9, bit12, bits13to15);
			}
			else {
				ADD(bits4to6, bit7, bits8to9, bits10to12, bits13to15);
			}

		}
		else if (bits0to3 == 14) {
			if (bits8to9 == 3) {
				if (bits4to6 == 0) {
					ASd(bit7, bits10to12, bits13to15);
				}
				else if (bits4to6 == 1) {
					LSd(bit7, bits10to12, bits13to15);
				}
				else if (bits4to6 == 2) {
					ROXd(bit7, bits10to12, bits13to15);
				}
				else {
					ROd(bit7, bits10to12, bits13to15);
				}
			}
			else {
				if (bits11to12 == 0) {
					ASd2(bit4to6, bit7, bits8to9, bits10, bits13to15);
				}
				else if (bits11to12 == 1) {
					LSd2(bit4to6, bit7, bits8to9, bits10, bits13to15);
				}
				else if (bits11to12 == 2) {
					ROXd2(bit4to6, bit7, bits8to9, bits10, bits13to15);
				}
				else {
					ROd2(bit4to6, bit7, bits8to9, bits10, bits13to15);
				}
			}
		}
		else {
		    std::cout << "Error in bits0to3" << std::endl;
		}

		std::cout << std::hex << instruction << std::endl;
}

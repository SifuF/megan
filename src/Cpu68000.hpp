#pragma once

#include "defs.hpp"
#include <array>
#include <bitset>
#include <cassert>
#include <cstddef>

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

enum Status {
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
    //void MOVE(uint8 src, uint8 dst, int size) {};
    //void ANDI();
    //void ADD();
    //void BEQ();
    //void LEA();
    //void DBRA();
    //void JMP();

    void ORItoCCR();
    void ORItoSR();
    void ORI(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void ANDItoCCR();
    void ANDItoSR();
    void ANDI(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void SUBI(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void ADDI(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void BTST(uint8 bits10to12, uint8 bits13to15);
    void BCHG(uint8 bits10to12, uint8 bits13to15);
    void BCLR(uint8 bits10to12, uint8 bits13to15);
    void BSET(uint8 bits10to12, uint8 bits13to15);
    void EORItoCCR();
    void EORItoSR();
    void EORI(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void CMPI(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void MOVEP(uint8 bits4to6, uint8 bit8, uint8 bit9, uint8 bits13to15);
    void BTST(uint8 bits4to6, uint8 bits10to12, uint8 bits13to15);
    void BCHG(uint8 bits4to6, uint8 bits10to12, uint8 bits13to15);
    void BCLR(uint8 bits4to6, uint8 bits10to12, uint8 bits13to15);
    void BSET(uint8 bits4to6, uint8 bits10to12, uint8 bits13to15);
    void MOVEA(uint8 bits2to3, uint8 bits4to6, uint8 bits10to12, uint8 bits13to15);
    void MOVE(uint8 bits2to3, uint8 bits4to6, uint8 bits7to9, uint8 bits10to12, uint8 bits13to15);
    void ILLEGAL();
    void TAS(uint8 bits10to12, uint8 bits13to15);
    void TST(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void TRAP(uint8 bits12to15);
    void LINK(uint8 bits13to15);
    void ULINK(uint8 bits13to15);
    void MOVEUSP(uint8 bit12, uint8 bits13to15);
    void RESET();
    void NOP();
    void STOP();
    void RTE();
    void RTS();
    void TRAPV();
    void RTR();
    void JSR(uint8 bits10to12, uint8 bits13to15);
    void JMP(uint8 bits10to12, uint8 bits13to15);
    void MOVEM(uint8 bit5, uint8 bit9, uint8 bits10to12, uint8 bits13to15);
    void LEA(uint8 bits4to6, uint8 bits10to12, uint8 bits13to15);
    void CHK(uint8 bits4to6, uint8 bits10to12, uint8 bits13to15);
    void EXT(uint8 bit9, uint8 bits13to15);
    void NBCD(uint8 bits10to12, uint8 bits13to15);
    void SWAP(uint8 bits13to15);
    void PEA(uint8 bits10to12, uint8 bits13to15);
    void MOVEfromSR(uint8 bits10to12, uint8 bits13to15);
    void MOVEtoCCR(uint8 bits10to12, uint8 bits13to15);
    void MOVEtoSR(uint8 bits10to12, uint8 bits13to15);
    void NEGX(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void CLR(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void NEG(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void NOT(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void DBcc(uint8 bits4to7, uint8 bits13to15);
    void Scc(uint8 bits4to7, uint8 bits10to12, uint8 bits13to15);
    void SUBQ(uint8 bits4to6, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void ADDQ(uint8 bits4to6, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void BRA(uint8 bits8to15);
    void BSR(uint8 bits8to15);
    void Bcc(uint8 bits4to7, uint8 bits8to15);
    void MOVEQ(uint8 bits4to6, uint8 bits8to15);
    void DIVS(uint8 bits4to6, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void DIVU(uint8 bits4to6, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void SBCD(uint8 bits4to6, uint8 bit12, uint8 bits13to15);
    void OR(uint8 bits4to6, uint8 bit7, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void SUBA(uint8 bits4to6, uint8 bit7, uint8 bits10to12, uint8 bits13to15);
    void SUBX(uint8 bits4to6, uint8 bits8to9, uint8 bit12, uint8 bits13to15);
    void SUB(uint8 bits4to6, uint8 bit7, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void CMPA(uint8 bits4to6, uint8 bit7, uint8 bits10to12, uint8 bits13to15);
    void CMPM(uint8 bits4to6, uint8 bits8to9, uint8 bits13to15);
    void CMP(uint8 bits4to6, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void EOR(uint8 bits4to6, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void ABCD(uint8 bits4to6, uint8 bit12, uint8 bits13to15);
    void MULU(uint8 bits4to6, uint8 bits10to12, uint8 bits13to15);
    void MULS(uint8 bits4to6, uint8 bits10to12, uint8 bits13to15);
    void EXG(uint8 bits4to6, uint8 bits8to9, uint8 bit12, uint8 bits13to15);
    void AND(uint8 bits4to6, uint8 bit7, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void ADDA(uint8 bits4to6, uint8 bit7, uint8 bits10to12, uint8 bits13to15);
    void ADDX(uint8 bits4to6, uint8 bits8to9, uint8 bit12, uint8 bits13to15);
    void ADD(uint8 bits4to6, uint8 bit7, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15);
    void ASd(uint8 bit7, uint8 bits10to12, uint8 bits13to15);
    void LSd(uint8 bit7, uint8 bits10to12, uint8 bits13to15);
    void ROXd(uint8 bit7, uint8 bits10to12, uint8 bits13to15);
    void ROd(uint8 bit7, uint8 bits10to12, uint8 bits13to15);
    void ASd2(uint8 bits4to6, uint8 bit7, uint8 bits8to9, uint8 bits10, uint8 bits13to15);
    void LSd2(uint8 bits4to6, uint8 bit7, uint8 bits8to9, uint8 bits10, uint8 bits13to15);
    void ROXd2(uint8 bits4to6, uint8 bit7, uint8 bits8to9, uint8 bits10, uint8 bits13to15);
    void ROd2(uint8 bits4to6, uint8 bit7, uint8 bits8to9, uint8 bits10, uint8 bits13to15);


    std::array<Reg, 8> D{};
    std::array<Reg, 8> A{};  //A7 is SP and SSP
    uint32 PC; // 24 uint8 bits

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
    std::bitset<16> SR; // 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
                        //  T     S       I2 I1 I0           X  N  Z  V  C

    Bus* bus;
};


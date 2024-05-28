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

        std::cout << std::hex << instruction << " ";

        const uint8 bits0to3 = static_cast<uint8>(instruction >> 12);
        const uint8 bits0to1 = static_cast<uint8>(instruction >> 14);
        const uint8 bits2to3 = static_cast<uint8>((instruction >> 12) & 0x3u);
        const uint8 bits4to6 = static_cast<uint8>((instruction >> 9) & 0x7u);
        const uint8 bit7 = static_cast<uint8>((instruction >> 8) & 0x1u);
        const uint8 bit8 = static_cast<uint8>((instruction >> 7) & 0x1u);
        const uint8 bit9 = static_cast<uint8>((instruction >> 6) & 0x1u);
        const uint8 bits8to15 = static_cast<uint8>(instruction);
        const uint8 bits8to9 = static_cast<uint8>((instruction >> 6) & 0x3u);
        const uint8 bits10to12 = static_cast<uint8>((instruction >> 3) & 0x7u);
        const uint8 bits13to15 = static_cast<uint8>((instruction) & 0x7u);
        const uint8 bits7to9 = static_cast<uint8>((instruction >> 6) & 0x7u);
        const uint16 bits4to15 = static_cast<uint16>((instruction) & 0x0FFFu);
        const uint8 bits4to9 = static_cast<uint8>((instruction >> 6) & 63u);
        const uint8 bits4to7 = static_cast<uint8>((instruction >> 8) & 0xF);
        const uint8 bits12to15 = static_cast<uint8>((instruction) & 0xF);
        const uint8 bits4to11 = static_cast<uint8>(instruction >> 4);
        const uint8 bit12 = static_cast<uint8>((instruction >> 3) & 0x1u);
        const uint8 bit4 = static_cast<uint8>((instruction >> 11) & 0x1u);
        const uint8 bit5 = static_cast<uint8>((instruction >> 10) & 0x1u);
        const uint8 bits6to8 = static_cast<uint8>((instruction >> 7) & 0x7u);
        const uint8 bits6to7 = static_cast<uint8>((instruction >> 8) & 0x3u);
        const uint8 bits7to8 = static_cast<uint8>((instruction >> 7) & 0x3u);
        const uint8 bits10to11 = static_cast<uint8>((instruction >> 4) & 0x3u);
        const uint8 bits7to11 = static_cast<uint8>((instruction >> 4) & 31u);
        const uint8 bits11to12 = static_cast<uint8>((instruction >> 3) & 0x3u);
        const uint8 bit10 = static_cast<uint8>((instruction >> 5) & 0x1u);

         switch (bits0to3) {
            case 0b0000: {
                if (bit7 == 0) {
                    switch (bits4to6) {
                        case 0: {
                            if (bits8to15 == 0b00111100) {
                                ORItoCCR();
                            }
                            else if (bits8to15 == 0b01111100) {
                                ORItoSR();
                            }
                            else {
                                ORI(bits8to9, bits10to12, bits13to15);
                            }
                            break;
                        }
                        case 1: {
                            if (bits8to15 == 0b00111100) {
                                ANDItoCCR();
                            }
                            else if (bits8to15 == 0b01111100) {
                                ANDItoSR();
                            }
                            else {
                                ANDI(bits8to9, bits10to12, bits13to15);
                            }
                            break;
                        }
                        case 2: {
                            SUBI(bits8to9, bits10to12, bits13to15);
                            break;
                        }
                        case 3: {
                            ADDI(bits8to9, bits10to12, bits13to15);
                            break;
                        }
                        case 4: {
                            switch (bits8to9) {
                                case 0: {
                                    BTST(bits10to12, bits13to15);
                                    break;
                                }
                                case 1: {
                                    BCHG(bits10to12, bits13to15);
                                    break;
                                }
                                case 2: {
                                    BCLR(bits10to12, bits13to15);
                                    break;
                                }
                                default: { // 3
                                    BSET(bits10to12, bits13to15);
                                    break;
                                }
                            }
                            break;
                        }
                        case 5: {
                            if (bits8to15 == 0b00111100) {
                                EORItoCCR();
                            }
                            else if (bits8to15 == 0b01111100) {
                                EORItoSR();
                            }
                            else {
                                EORI(bits8to9, bits10to12, bits13to15);
                            }
                            break;
                        }
                        case 6: {
                            CMPI(bits8to9, bits10to12, bits13to15);
                            break;
                        }
                        default: {
                            std::cout << "Error" << std::endl;
                            break;
                        }
                    }
                }
                else {
                    if (bits10to12 == 1) {
                        MOVEP(bits4to6, bit8, bit9, bits13to15);
                    }
                    else {
                        switch (bits8to9) {
                            case 0: {
                                BTST(bits4to6, bits10to12, bits13to15);
                                break;
                            }
                            case 1: {
                                BCHG(bits4to6, bits10to12, bits13to15);
                                break;
                            }
                            case 2: {
                                BCLR(bits4to6, bits10to12, bits13to15);
                                break;
                            }
                            default: {
                                BSET(bits4to6, bits10to12, bits13to15);
                                break;
                            }
                        }
                    }
                }
                break;
            }
            case 0b0001: [[fallthrough]];
            case 0b0010: [[fallthrough]];
            case 0b0011: {
                if (bits7to9 == 1) {
                    MOVEA(bits2to3, bits4to6, bits10to12, bits13to15);
                }
                else {
                    MOVE(bits2to3, bits4to6, bits7to9, bits10to12, bits13to15);
                }
                break;
            }
            case 0b0100: {
                if (bits4to15 == 0b101011111100) {
                    ILLEGAL();
                }
                else if (bits4to9 == 0b101011) {
                    TAS(bits10to12, bits13to15);
                }
                else if (bits4to7 == 0b1010) {
                    TST(bits8to9, bits10to12, bits13to15);
                }
                else if (bits4to11 == 0b11100100) {
                    TRAP(bits12to15);
                }
                else if (bits4to11 == 0b11001010 && bit12 == 1) {
                    LINK(bits13to15);
                }
                else if (bits4to11 == 0b11001011 && bit12 == 1) {
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
                else if (bits4to9 == 0b111011) {
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
                break;
            }
            case 5: {
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
                break;
            }
            case 6: {
                if (bits4to7 == 0) {
                    BRA(bits8to15);
                }
                else if (bits4to7 == 1) {
                    BSR(bits8to15);
                }
                else {
                    Bcc(bits4to7, bits8to15);
                }
                break;
            }
            case 7: {
                MOVEQ(bits4to6, bits8to15);
                break;
            }
            case 8: {
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
                break;
            }
            case 9: {
                if (bits8to9 == 3) {
                    SUBA(bits4to6, bit7, bits10to12, bits13to15);
                }
                else if (bit7 == 1 && bits10to11 == 0) {
                    SUBX(bits4to6, bits8to9, bit12, bits13to15);
                }
                else {
                    SUB(bits4to6, bit7, bits8to9, bits10to12, bits13to15);
                }
                break;
            }
            case 11: {
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
                break;
            }
            case 12: {
                if (bits7to11 == 16) {
                    ABCD(bits4to6, bit12, bits13to15);
                }
                else if (bit7 == 0 && bits8to9 == 3) {
                    MULU(bits4to6, bits10to12, bits13to15);
                }
                else if (bit7 == 1 && bits8to9 == 3) {
                    MULS(bits4to6, bits10to12, bits13to15);
                }
                else if (bit7 == 1 && bits10to11 == 0) {
                    EXG(bits4to6, bits8to9, bit12, bits13to15);
                }
                else {
                    AND(bits4to6, bit7, bits8to9, bits10to12, bits13to15);
                }
                break;
            }
            case 13: {
                if (bits8to9 == 3) {
                    ADDA(bits4to6, bit7, bits10to12, bits13to15);
                }
                else if (bit7 == 1 && bits10to11 == 0) {
                    ADDX(bits4to6, bits8to9, bit12, bits13to15);
                }
                else {
                    ADD(bits4to6, bit7, bits8to9, bits10to12, bits13to15);
                }
                break;

            }
            case 14: {
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
                        ASd2(bits4to6, bit7, bits8to9, bit10, bits13to15);
                    }
                    else if (bits11to12 == 1) {
                        LSd2(bits4to6, bit7, bits8to9, bit10, bits13to15);
                    }
                    else if (bits11to12 == 2) {
                        ROXd2(bits4to6, bit7, bits8to9, bit10, bits13to15);
                    }
                    else {
                        ROd2(bits4to6, bit7, bits8to9, bit10, bits13to15);
                    }
                }
                break;
            }
            default: {
                std::cout << "Error in bits0to3" << std::endl;
            }
        }

}

void Cpu68000::ORItoCCR() {}
void Cpu68000::ORItoSR() {}
void Cpu68000::ORI(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::ANDItoCCR() {}
void Cpu68000::ANDItoSR() {}
void Cpu68000::ANDI(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {
    const uint16 value = bus->readWord(PC);
    PC += 2;
    D[0].b &= static_cast<uint8>(value);
    if (D[0].b == 0) {
        SR.set(StatusRegister::Z, true);
    }
    else {
        SR.set(StatusRegister::Z, false);
    }

    std::cout << "andi.b " << std::hex << value << " " << int(D[0].b) << std::endl;
}
void Cpu68000::SUBI(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::ADDI(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {
    std::cout << "addi";

    auto getAddressingMode = [](uint8 memBits, uint8 regBits) -> AddressingMode {
        AddressingMode addressingMode = AddressingMode::UnknownAddress;
        switch (memBits) {
            case 0b000: {
                addressingMode = AddressingMode::DataRegister;
                std::cout << "D[" << static_cast<int>(regBits) << "]";
                break;
            }
            case 0b001: { // Not valid 68k?
                addressingMode = AddressingMode::AddressRegister;
                std::cout << "A[" << static_cast<int>(regBits) << "]";
                break;
            }
            case 0b010: {
                addressingMode = AddressingMode::Address;
                std::cout << "(A[" << static_cast<int>(regBits) << "])";
                break;
            }
            case 0b011: {
                addressingMode = AddressingMode::AddressPostIncrement;
                std::cout << "(A[" << static_cast<int>(regBits) << "])+";
                break;
            }
            case 0b100: {
                addressingMode = AddressingMode::AddressPreDecrement;
                std::cout << "-(A[" << static_cast<int>(regBits) << "])";
                break;
            }
            default: {
                std::cout << "ADDI needs upgrade!";
                break;
            }
        }
        return addressingMode;
    };

    auto write = [this](AddressingMode addressingMode, OperationSize operationSize, int reg, uint32 value) {
        switch (addressingMode) {
            case AddressingMode::DataRegister: {
                if (operationSize == OperationSize::Byte) {
                    D[reg].b += value;
                }
                else if (operationSize == OperationSize::Word) {
                    D[reg].w += value;
                }
                else if (operationSize == OperationSize::Long) {
                    D[reg].l += value;
                }
                break;
            }
            case AddressingMode::AddressRegister: { // Not valid 68k?
                if (operationSize == OperationSize::Byte) {
                    A[reg].b += value;
                }
                else if (operationSize == OperationSize::Word) {
                    A[reg].w += value;
                }
                else if (operationSize == OperationSize::Long) {
                    A[reg].l += value;
                }
                break;
            }
            case AddressingMode::Address: {
                if (operationSize == OperationSize::Byte) {
                    const auto read = bus->readByte(A[reg].b);
                    bus->writeByte(A[reg].b, read + value);
                }
                else if (operationSize == OperationSize::Word) {
                    const auto read = bus->readWord(A[reg].w);
                    bus->writeByte(A[reg].w, read + value);
                }
                else if (operationSize == OperationSize::Long) {
                    const auto read = bus->readLong(A[reg].l);
                    bus->writeByte(A[reg].l, read + value);
                }
                break;
            }
            case AddressingMode::AddressPostIncrement: {
                if (operationSize == OperationSize::Byte) {
                    const auto read = bus->readByte(A[reg].b);
                    bus->writeByte(A[reg].b, read + value);
                    A[reg].b++;
                }
                else if (operationSize == OperationSize::Word) {
                    const auto read = bus->readWord(A[reg].w);
                    bus->writeByte(A[reg].w, read + value);
                    A[reg].w += 2;
                }
                else if (operationSize == OperationSize::Long) {
                    const auto read = bus->readLong(A[reg].l);
                    bus->writeByte(A[reg].l, read + value);
                    A[reg].l += 4;
                }
                break;
            }
            case AddressingMode::AddressPreDecrement: {
                if (operationSize == OperationSize::Byte) {
                    A[reg].b--;
                    const auto read = bus->readByte(A[reg].b);
                    bus->writeByte(A[reg].b, read + value);
                }
                else if (operationSize == OperationSize::Word) {
                    A[reg].w -=2;
                    const auto read = bus->readWord(A[reg].w);
                    bus->writeByte(A[reg].w, read + value);
                }
                else if (operationSize == OperationSize::Long) {
                    A[reg].l -= 4;
                    const auto read = bus->readLong(A[reg].l);
                    bus->writeByte(A[reg].l, read + value);
                }
                break;
            }
            case AddressingMode::AddressDisplacement: {
                break;
            }
            case AddressingMode::AddressIndex: {
                break;
            }
            case AddressingMode::ProgramCounterDisplacement: {
                break;
            }
            case AddressingMode::ProgramCounterIndex: {
                break;
            }
            case AddressingMode::AbsoluteShort: {
                break;
            }
            case AddressingMode::AbsoluteLong: {
                break;
            }
            default: {
                throw std::exception("Unknown addressing mode!");
                break;
            }
        }
    };

    OperationSize operationSize = OperationSize::UnknownSize;
    if (bits8to9 == 0) {
        operationSize = OperationSize::Byte;
        std::cout << ".b ";
    }
    else if (bits8to9 == 1) {
        operationSize = OperationSize::Word;
        std::cout << ".w ";
    }
    else if (bits8to9 == 2) {
        operationSize = OperationSize::Long;
        std::cout << ".l ";
    }

    const auto reg = static_cast<int>(bits13to15);
    const auto value = bus->readLong(PC);
    PC += 4;
    std::cout << value << ", ";

    AddressingMode addressingMode = getAddressingMode(bits10to12, bits13to15);

    write(addressingMode, operationSize, reg, value);
    
    std::cout << std::endl;
}
void Cpu68000::BTST(uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::BCHG(uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::BCLR(uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::BSET(uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::EORItoCCR() {}
void Cpu68000::EORItoSR() {}
void Cpu68000::EORI(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::CMPI(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::MOVEP(uint8 bits4to6, uint8 bit8, uint8 bit9, uint8 bits13to15) {}
void Cpu68000::BTST(uint8 bits4to6, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::BCHG(uint8 bits4to6, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::BCLR(uint8 bits4to6, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::BSET(uint8 bits4to6, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::MOVEA(uint8 bits2to3, uint8 bits4to6, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::MOVE(uint8 bits2to3, uint8 bits4to6, uint8 bits7to9, uint8 bits10to12, uint8 bits13to15) {
    std::cout << "move";

    // instruction: 1039
    // bits2to3 == 1 => move.b
    // bits4to6 == 0 => to reg 0
    // bits7to9 == 0 => data reg
    // bits10to12 == 7 => from see below
    // bits13to15 == 1 => Absolute long

    // instruction: 23fc
    // bits2to3 == 2 => move.l
    // bits4to6 == 1 => (would be if not for below) to reg 1
    // bits7to9 == 7 => absolute long (write to memory address)
    // bits10to12 == 7 => from see below
    // bits13to15 == 1 => immediate

    auto getAddressingMode = [this](uint8 memBits, uint8 regBits) -> AddressingMode {
        AddressingMode addressingMode = AddressingMode::UnknownAddress;
        switch (memBits) {
            case 0b000: {
                addressingMode = AddressingMode::DataRegister;
                std::cout << "D[" << static_cast<int>(regBits) << "] ";
                break;
            }
            case 0b001: {
                addressingMode = AddressingMode::AddressRegister;
                std::cout << "A[" << static_cast<int>(regBits) << "] ";
                break;
            }
            case 0b010: {
                addressingMode = AddressingMode::Address;
                std::cout << "(A[" << static_cast<int>(regBits) << "]) ";
                break;
            }
            case 0b011: {
                addressingMode = AddressingMode::AddressPostIncrement;
                std::cout << "(A[" << static_cast<int>(regBits) << "])+ ";
                break;
            }
            case 0b100: {
                addressingMode = AddressingMode::AddressPreDecrement;
                std::cout << "-(A[" << static_cast<int>(regBits) << "]) ";
                break;
            }
            case 0b101: {
                addressingMode = AddressingMode::AddressDisplacement;
                std::cout << "displacement(A[" << static_cast<int>(regBits) << "]) ";
                break;
            }
            case 0b110: {
                addressingMode = AddressingMode::AddressIndex;
                std::cout << "index(A[" << static_cast<int>(regBits) << "]) ";
                break;
            }
            case 0b111: {
                switch (regBits) {
                    case 0b010: {
                        addressingMode = AddressingMode::ProgramCounterDisplacement;
                        std::cout << "PC.displacement ";
                        break;
                    }
                    case 0b011: {
                        addressingMode = AddressingMode::ProgramCounterIndex;
                        std::cout << "PC.index ";
                        break;
                    }
                    case 0b000: {
                        addressingMode = AddressingMode::AbsoluteShort;
                        std::cout << "Abs.w ";
                        break;
                    }
                    case 0b001: {
                        addressingMode = AddressingMode::AbsoluteLong;
                        std::cout << "Abs.l ";
                        break;
                    }
                    case 0b100: {
                        addressingMode = AddressingMode::Immediate;
                        std::cout << "Immediate ";
                        break;
                    }
                }
            }
        }
        return addressingMode;
    };

    auto read = [this](AddressingMode addressingMode, OperationSize operationSize, int reg) -> uint32 {
        uint32 value = 0;
        switch (addressingMode) {
            case AddressingMode::DataRegister: {
                if (operationSize == OperationSize::Byte) {
                    value = D[reg].b;
                }
                else if (operationSize == OperationSize::Word) {
                    value = D[reg].w;
                }
                else if (operationSize == OperationSize::Long) {
                    value = D[reg].l;
                }
                break;
            }
            case AddressingMode::AddressRegister: {
                if (operationSize == OperationSize::Byte) {
                    value = A[reg].b;
                }
                else if (operationSize == OperationSize::Word) {
                    value = A[reg].w;
                }
                else if (operationSize == OperationSize::Long) {
                    value = A[reg].l;
                }
                break;
            }
            case AddressingMode::Address: {
                if (operationSize == OperationSize::Byte) {
                    value = bus->readByte(A[reg].b); // expansion
                }
                else if (operationSize == OperationSize::Word) {
                    value = bus->readWord(A[reg].w); // expansion
                }
                else if (operationSize == OperationSize::Long) {
                    value = bus->readLong(A[reg].l);
                }
                break;
            }
            case AddressingMode::AddressPostIncrement: {
                if (operationSize == OperationSize::Byte) {
                    value = bus->readByte(A[reg].b); // expansion
                    A[reg].b++;
                }
                else if (operationSize == OperationSize::Word) {
                    value = bus->readWord(A[reg].w); // expansion
                    A[reg].w += 2;
                }
                else if (operationSize == OperationSize::Long) {
                    value = bus->readLong(A[reg].l);
                    A[reg].l += 4;
                }
                break;
            }
            case AddressingMode::AddressPreDecrement: {
                if (operationSize == OperationSize::Byte) {
                    A[reg].b--;
                    value = bus->readByte(A[reg].b); // expansion
                }
                else if (operationSize == OperationSize::Word) {
                    A[reg].w -= 2;
                    value = bus->readWord(A[reg].w); // expansion
                }
                else if (operationSize == OperationSize::Long) {
                    A[reg].l -= 4;
                    value = bus->readLong(A[reg].l);
                }
                break;
            }
            case AddressingMode::AddressDisplacement: {
                break;
            }
            case AddressingMode::AddressIndex: {
                break;
            }
            case AddressingMode::ProgramCounterDisplacement: {
                break;
            }
            case AddressingMode::ProgramCounterIndex: {
                break;
            }
            case AddressingMode::AbsoluteShort: {
                value = bus->readWord(PC); // expansion
                PC += 2;
                break;
            }
            case AddressingMode::AbsoluteLong: {
                value = bus->readLong(PC);
                PC += 4;
                break;
            }
            case AddressingMode::Immediate: {
                if (operationSize == OperationSize::Byte) {
                    // invalid
                }
                else if (operationSize == OperationSize::Word) {
                    value = bus->readWord(PC);
                    PC += 2;
                }
                else if (operationSize == OperationSize::Long) {
                    value = bus->readLong(PC);
                    PC += 4;
                }
                break;
            }
            default: {
                throw std::exception("Unknown addressing mode!");
                break;
            }
        }
        return value;
    };

    auto write = [this](AddressingMode addressingMode, OperationSize operationSize, uint8 reg, uint32 value) {
        switch (addressingMode) {
            case AddressingMode::DataRegister: {
                if (operationSize == OperationSize::Byte) {
                    D[reg].b = value;
                }
                else if (operationSize == OperationSize::Word) {
                    D[reg].w = value;
                }
                else if (operationSize == OperationSize::Long) {
                    D[reg].l = value;
                }
                break;
            }
            case AddressingMode::AddressRegister: {
                if (operationSize == OperationSize::Byte) {
                    A[reg].b = value;
                }
                else if (operationSize == OperationSize::Word) {
                    A[reg].w = value;
                }
                else if (operationSize == OperationSize::Long) {
                    A[reg].l = value;
                }
                break;
            }
            case AddressingMode::Address: {
                if (operationSize == OperationSize::Byte) {
                    bus->writeByte(A[reg].b, value);
                }
                else if (operationSize == OperationSize::Word) {
                    bus->writeWord(A[reg].w, value);
                }
                else if (operationSize == OperationSize::Long) {
                    bus->writeLong(A[reg].l, value);
                }
                break;
            }
            case AddressingMode::AddressPostIncrement: {
                if (operationSize == OperationSize::Byte) {
                    bus->writeByte(A[reg].b, value);
                    A[reg].b++;
                }
                else if (operationSize == OperationSize::Word) {
                    bus->writeWord(A[reg].w, value);
                    A[reg].w += 2;
                }
                else if (operationSize == OperationSize::Long) {
                    bus->writeLong(A[reg].l, value);
                    A[reg].l += 4;
                }
                break;
            }
            case AddressingMode::AddressPreDecrement: {
                if (operationSize == OperationSize::Byte) {
                    A[reg].b--;
                    bus->writeByte(A[reg].b, value);
                }
                else if (operationSize == OperationSize::Word) {
                    A[reg].w -= 2;
                    bus->writeWord(A[reg].w, value);
                }
                else if (operationSize == OperationSize::Long) {
                    A[reg].l -= 4;
                    bus->writeLong(A[reg].l, value);
                }
                break;
            }
            case AddressingMode::AddressDisplacement: {
                break;
            }
            case AddressingMode::AddressIndex: {
                break;
            }
            case AddressingMode::ProgramCounterDisplacement: {
                break;
            }
            case AddressingMode::ProgramCounterIndex: {
                break;
            }
            case AddressingMode::AbsoluteShort: {
                const auto address = bus->readWord(PC); // expansion
                PC += 2;
                if (operationSize == OperationSize::Byte) {
                    bus->writeByte(address, value);
                }
                else if (operationSize == OperationSize::Word) {
                    bus->writeWord(address, value);
                }
                else if (operationSize == OperationSize::Long) {
                    bus->writeLong(address, value);
                }
                break;
            }
            case AddressingMode::AbsoluteLong: {
                const auto address = bus->readLong(PC);
                PC += 4;
                if (operationSize == OperationSize::Byte) {
                    bus->writeByte(address, value);
                }
                else if (operationSize == OperationSize::Word) {
                    bus->writeWord(address, value);
                }
                else if (operationSize == OperationSize::Long) {
                    bus->writeLong(address, value);
                }
                break;
            }
            case AddressingMode::Immediate: {
                const auto address = bus->readLong(PC);
                PC += 4;
                if (operationSize == OperationSize::Byte) {
                    bus->writeByte(address, value);
                }
                else if (operationSize == OperationSize::Word) {
                    bus->writeWord(address, value);
                }
                else if (operationSize == OperationSize::Long) {
                    bus->writeLong(address, value);
                }
                break;
            }
            default: {
                throw std::exception("Unknown addressing mode!");
                break;
            }
        }
    };

    OperationSize operationSize = OperationSize::UnknownSize;
    if (bits2to3 == 1) {
        operationSize = OperationSize::Byte;
        std::cout << ".b ";
    }
    else if (bits2to3 == 2) {
        operationSize = OperationSize::Long;
        std::cout << ".l ";
    }
    else if (bits2to3 == 3) {
        operationSize = OperationSize::Word;
        std::cout << ".w ";
    }

    AddressingMode dstAddressingMode = getAddressingMode(bits7to9, bits4to6);
    AddressingMode srcAddressingMode = getAddressingMode(bits10to12, bits13to15);

    uint32 srcValue = read(srcAddressingMode, operationSize, bits13to15);
    write(dstAddressingMode, operationSize, bits4to6, srcValue);

    std::cout << std::endl;
}
void Cpu68000::ILLEGAL() {}
void Cpu68000::TAS(uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::TST(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::TRAP(uint8 bits12to15) {}
void Cpu68000::LINK(uint8 bits13to15) {}
void Cpu68000::ULINK(uint8 bits13to15) {}
void Cpu68000::MOVEUSP(uint8 bit12, uint8 bits13to15) {}
void Cpu68000::RESET() {}
void Cpu68000::NOP() {}
void Cpu68000::STOP() {}
void Cpu68000::RTE() {}
void Cpu68000::RTS() {}
void Cpu68000::TRAPV() {}
void Cpu68000::RTR() {}
void Cpu68000::JSR(uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::JMP(uint8 bits10to12, uint8 bits13to15) {
    const uint32 value = bus->readLong(PC);
    PC = value;
    std::cout << "jmp " << static_cast<int>(value) << std::endl;
}
void Cpu68000::MOVEM(uint8 bit5, uint8 bit9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::LEA(uint8 bits4to6, uint8 bits10to12, uint8 bits13to15) {
    //a0
    //7
    //1 abs long
    const int reg = static_cast<int>(bits4to6);
    std::cout << "lea ";
    if (bits10to12 == 7) { // should do always?
        if (bits13to15 == 1) { // Abs.l
            const uint32 value = bus->readLong(PC);
            PC += 4;
            A[reg].l = value;
            std::cout << value << ", A[" << reg << "] " << std::endl;
        }
    }
}
void Cpu68000::CHK(uint8 bits4to6, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::EXT(uint8 bit9, uint8 bits13to15) {}
void Cpu68000::NBCD(uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::SWAP(uint8 bits13to15) {}
void Cpu68000::PEA(uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::MOVEfromSR(uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::MOVEtoCCR(uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::MOVEtoSR(uint8 bits10to12, uint8 bits13to15) {
    const uint32 value = bus->readLong(PC);
    PC += 4;
    SR = value;
    std::cout << "movetoSR " << static_cast<int>(value) << std::endl;
}
void Cpu68000::NEGX(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::CLR(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::NEG(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::NOT(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::DBcc(uint8 bits4to7, uint8 bits13to15) {
    std::cout << "dbra_";
    const int condition = static_cast<int>(bits4to7);
    const int reg = static_cast<int>(bits13to15);
    const uint16 value = bus->readWord(PC);
    const short displacement = static_cast<short>(value);
    PC += 2; // might need to undo
    switch (condition) {
        case 0b0000: {
            std::cout << "true";
            break;
        }
        case 0b0001: {
            std::cout << "false D[" << reg << "], " << value << " " << std::dec << displacement << std::hex;
            if (D[reg].w == 0) {
                // continue to next instruction
            }
            else {
                D[reg].w--;
                PC += displacement - 2; // undo 
            }
            break;
        }
        case 0b0010: {
            std::cout << "higher";
            break;
        }
        case 0b0011: {
            std::cout << "lowerOrSame";
            break;
        }
        case 0b0100: {
            std::cout << "CarryClear";
            break;
        }
        case 0b0101: {
            std::cout << "CarrySet";
            break;
        }
        case 0b0110: {
            std::cout << "NotEqual";
            break;
        }
        case 0b0111: {
            std::cout << "Equal";
            break;
        }
        case 0b1000: {
            std::cout << "OverflowClear";
            break;
        }
        case 0b1001: {
            std::cout << "OverflowSet";
            break;
        }
        case 0b1010: {
            std::cout << "Plus";
            break;
        }
        case 0b1011: {
            std::cout << "Minus";
            break;
        }
        case 0b1100: {
            std::cout << "GreaterOrEqual";
            break;
        }
        case 0b1101: {
            std::cout << "LessThan";
            break;
        }
        case 0b1110: {
            std::cout << "GreaterThan";
            break;
        }
        case 0b1111: {
            std::cout << "LessOrEqual";
            break;
        }
    }
    std::cout << std::endl;
}
void Cpu68000::Scc(uint8 bits4to7, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::SUBQ(uint8 bits4to6, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::ADDQ(uint8 bits4to6, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::BRA(uint8 bits8to15) {}
void Cpu68000::BSR(uint8 bits8to15) {}
void Cpu68000::Bcc(uint8 bits4to7, uint8 bits8to15) {
    std::cout << "b_";
    const int condition = static_cast<int>(bits4to7);
    const char displacementOpcode = static_cast<char>(bits8to15);
    const uint16 displacementFollowing = bus->readWord(PC);
    PC += 2;

    const uint32 displacement = (displacementOpcode << 16) | displacementFollowing;

    switch (condition) {
    case 0b0000: {
        std::cout << "true";
        break;
    }
    case 0b0001: {
        std::cout << "false";
        break;
    }
    case 0b0010: {
        std::cout << "higher";
        break;
    }
    case 0b0011: {
        std::cout << "lowerOrSame";
        break;
    }
    case 0b0100: {
        std::cout << "CarryClear";
        break;
    }
    case 0b0101: {
        std::cout << "CarrySet";
        break;
    }
    case 0b0110: {
        std::cout << "NotEqual";
        break;
    }
    case 0b0111: {
        std::cout << "Equal ";
        std::cout << "displacement:" << static_cast<int>(displacement);
        if (SR[StatusRegister::Z] == 1) {
            PC += displacement - 2;
        }
        else {
            // continue
        }
        break;
    }
    case 0b1000: {
        std::cout << "OverflowClear";
        break;
    }
    case 0b1001: {
        std::cout << "OverflowSet";
        break;
    }
    case 0b1010: {
        std::cout << "Plus";
        break;
    }
    case 0b1011: {
        std::cout << "Minus";
        break;
    }
    case 0b1100: {
        std::cout << "GreaterOrEqual";
        break;
    }
    case 0b1101: {
        std::cout << "LessThan";
        break;
    }
    case 0b1110: {
        std::cout << "GreaterThan";
        break;
    }
    case 0b1111: {
        std::cout << "LessOrEqual";
        break;
    }
    }
    std::cout << std::endl;
}
void Cpu68000::MOVEQ(uint8 bits4to6, uint8 bits8to15) {
    const int reg = static_cast<int>(bits4to6);
    D[reg].l = bits8to15;
    std::cout << "moveq " <<static_cast<int>(bits8to15) << ", D[" << reg << "]" << std::endl;
}
void Cpu68000::DIVS(uint8 bits4to6, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::DIVU(uint8 bits4to6, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::SBCD(uint8 bits4to6, uint8 bit12, uint8 bits13to15) {}
void Cpu68000::OR(uint8 bits4to6, uint8 bit7, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::SUBA(uint8 bits4to6, uint8 bit7, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::SUBX(uint8 bits4to6, uint8 bits8to9, uint8 bit12, uint8 bits13to15) {}
void Cpu68000::SUB(uint8 bits4to6, uint8 bit7, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::CMPA(uint8 bits4to6, uint8 bit7, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::CMPM(uint8 bits4to6, uint8 bits8to9, uint8 bits13to15) {}
void Cpu68000::CMP(uint8 bits4to6, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::EOR(uint8 bits4to6, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::ABCD(uint8 bits4to6, uint8 bit12, uint8 bits13to15) {}
void Cpu68000::MULU(uint8 bits4to6, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::MULS(uint8 bits4to6, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::EXG(uint8 bits4to6, uint8 bits8to9, uint8 bit12, uint8 bits13to15) {}
void Cpu68000::AND(uint8 bits4to6, uint8 bit7, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::ADDA(uint8 bits4to6, uint8 bit7, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::ADDX(uint8 bits4to6, uint8 bits8to9, uint8 bit12, uint8 bits13to15) {}
void Cpu68000::ADD(uint8 bits4to6, uint8 bit7, uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {
    std::cout << "add";

    std::cout << std::endl;
}
void Cpu68000::ASd(uint8 bit7, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::LSd(uint8 bit7, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::ROXd(uint8 bit7, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::ROd(uint8 bit7, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::ASd2(uint8 bits4to6, uint8 bit7, uint8 bits8to9, uint8 bits10, uint8 bits13to15) {}
void Cpu68000::LSd2(uint8 bits4to6, uint8 bit7, uint8 bits8to9, uint8 bits10, uint8 bits13to15) {}
void Cpu68000::ROXd2(uint8 bits4to6, uint8 bit7, uint8 bits8to9, uint8 bits10, uint8 bits13to15) {}
void Cpu68000::ROd2(uint8 bits4to6, uint8 bit7, uint8 bits8to9, uint8 bits10, uint8 bits13to15) {}

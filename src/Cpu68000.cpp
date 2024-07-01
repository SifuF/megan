#include "Cpu68000.hpp"

#include "Bus.hpp"

#include <iostream>
#include <sstream>

Cpu68000::Cpu68000(Bus* b) : bus(b), PC(0), SR(0) {
}

Cpu68000::~Cpu68000() {}

void Cpu68000::reset() {
    for (int i = 0; i < 8; i++) {
        A[i].l = 0x0;
        D[i].l = 0x0;
    }
    SR = 0x0;
    A[7].l = bus->read<uint32>(0x000000);
    PC = bus->read<uint32>(0x000004);
}


void Cpu68000::fetch() {
    static int instructionCounter = -1;
    instructionCounter++;

    const uint16 instruction = bus->read<uint16>(PC);
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

    if (instructionCounter == 100) {
        std::cout << "break here" << std::endl;
    }

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
    std::stringstream preStream, srcStream, dstStream, postStream;
    preStream << "andi";

    auto statusUpdate = [this](unsigned lastValue) {
        if (lastValue == 0) {
            SR.set(StatusRegister::Z, true);
        }
        else {
            SR.set(StatusRegister::Z, false);
        }
    };

    auto write = [this, &statusUpdate](AddressingMode addressingMode, OperationSize operationSize, uint8 reg, uint32 value, std::stringstream& ss) {
        ss << std::hex;
        switch (addressingMode) {
            case AddressingMode::DataRegister: {
                ss << "D" << static_cast<int>(reg);
                if (operationSize == OperationSize::Byte) {
                    D[reg].b &= static_cast<uint8>(value);
                    statusUpdate(D[reg].b);
                }
                else if (operationSize == OperationSize::Word) {
                    D[reg].w &= static_cast<uint16>(value);
                    statusUpdate(D[reg].w);
                }
                else if (operationSize == OperationSize::Long) {
                    D[reg].l &= static_cast<uint32>(value);
                    statusUpdate(D[reg].l);
                }
                break;
            }
            case AddressingMode::AddressRegister: {
                ss << "A" << static_cast<int>(reg);
                if (operationSize == OperationSize::Byte) {
                    A[reg].b &= static_cast<uint8>(value);
                    statusUpdate(A[reg].b);
                }
                else if (operationSize == OperationSize::Word) {
                    A[reg].w &= static_cast<uint16>(value);
                    statusUpdate(A[reg].w);
                }
                else if (operationSize == OperationSize::Long) {
                    A[reg].l &= static_cast<uint32>(value);
                    statusUpdate(A[reg].l);
                }
                break;
            }
            case AddressingMode::Address: {
                ss << "(A" << static_cast<int>(reg) << ")";
                if (operationSize == OperationSize::Byte) {
                    const auto read = bus->read<uint8>(A[reg].l);
                    bus->write<uint8>(A[reg].l, (read & static_cast<uint8>(value)));
                }
                else if (operationSize == OperationSize::Word) {
                    const auto read = bus->read<uint16>(A[reg].l);
                    bus->write<uint16>(A[reg].l, (read & static_cast<uint16>(value)));
                }
                else if (operationSize == OperationSize::Long) {
                    const auto read = bus->read<uint32>(A[reg].l);
                    bus->write<uint32>(A[reg].l, (read & static_cast<uint32>(value)));
                }
                break;
            }
            case AddressingMode::AddressPostIncrement: {
                ss << "(A" << static_cast<int>(reg) << ")+";
                if (operationSize == OperationSize::Byte) {
                    const auto read = bus->read<uint8>(A[reg].l);
                    bus->write<uint8>(A[reg].l, (read & static_cast<uint8>(value)));
                    A[reg].b++;
                }
                else if (operationSize == OperationSize::Word) {
                    const auto read = bus->read<uint16>(A[reg].l);
                    bus->write<uint16>(A[reg].l, (read & static_cast<uint16>(value)));
                    A[reg].w += 2;
                }
                else if (operationSize == OperationSize::Long) {
                    const auto read = bus->read<uint32>(A[reg].l);
                    bus->write<uint32>(A[reg].l, (read & static_cast<uint32>(value)));
                    A[reg].l += 4;
                }
                break;
            }
            case AddressingMode::AddressPreDecrement: {
                ss << "-(A" << static_cast<int>(reg) << ")";
                if (operationSize == OperationSize::Byte) {
                    A[reg].b--;
                    const auto read = bus->read<uint8>(A[reg].l);
                    bus->write<uint8>(A[reg].l, (read & static_cast<uint8>(value)));
                }
                else if (operationSize == OperationSize::Word) {
                    A[reg].w -= 2;
                    const auto read = bus->read<uint16>(A[reg].l);
                    bus->write<uint16>(A[reg].l, (read & static_cast<uint16>(value)));
                }
                else if (operationSize == OperationSize::Long) {
                    A[reg].l -= 4;
                    const auto read = bus->read<uint32>(A[reg].l);
                    bus->write<uint32>(A[reg].l, (read & static_cast<uint32>(value)));
                }
                break;
            }
            case AddressingMode::AddressDisplacement: {
                ss << "displacement(A" << static_cast<int>(reg) << ") ";
                break;
            }
            case AddressingMode::AddressIndex: {
                ss << "index(A" << static_cast<int>(reg) << ") ";
                break;
            }
            case AddressingMode::ProgramCounterDisplacement: {
                ss << "PC.displacement ";
                break;
            }
            case AddressingMode::ProgramCounterIndex: {
                ss << "index(A" << static_cast<int>(reg) << ") ";
                break;
            }
            case AddressingMode::AbsoluteShort: {
                throw std::runtime_error("AbsoluteShort addi!");
            }
            case AddressingMode::AbsoluteLong: {
                throw std::runtime_error("AbsoluteLong addi!");
            }
            case AddressingMode::Immediate: {
                throw std::runtime_error("Immediate mode write!");
            }
            default: {
                throw std::exception("Unknown addressing mode!");
            }
        }
    };

    OperationSize operationSize = OperationSize::UnknownSize;
    if (bits8to9 == 0) {
        operationSize = OperationSize::Byte;
        preStream << ".b";
    }
    else if (bits8to9 == 1) {
        operationSize = OperationSize::Word;
        preStream << ".w";
    }
    else if (bits8to9 == 2) {
        operationSize = OperationSize::Long;
        preStream << ".l";
    }

    const uint16 value = bus->read<uint16>(PC);
    PC += 2;
    srcStream << std::hex << "$" << value;

    AddressingMode dstAddressingMode = getAddressingMode(bits10to12, bits13to15);
    write(dstAddressingMode, operationSize, bits13to15, value, dstStream);

    postStream << '\n';
    std::cout << preStream.str() << " " << srcStream.str() << ", " << dstStream.str() << postStream.str();
}
void Cpu68000::SUBI(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {}
void Cpu68000::ADDI(uint8 bits8to9, uint8 bits10to12, uint8 bits13to15) {
    std::stringstream preStream, srcStream, dstStream, postStream;
    preStream << "addi";

    auto write = [this](AddressingMode addressingMode, OperationSize operationSize, int reg, uint32 value, std::stringstream & ss) {
        ss << std::hex;
        switch (addressingMode) {
            case AddressingMode::DataRegister: {
                ss << "D" << static_cast<int>(reg);
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
                ss << "A" << static_cast<int>(reg);
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
                ss << "(A" << static_cast<int>(reg) << ")";
                if (operationSize == OperationSize::Byte) {
                    const auto read = bus->read<uint8>(A[reg].b);
                    bus->write<uint8>(A[reg].b, read + value);
                }
                else if (operationSize == OperationSize::Word) {
                    const auto read = bus->read<uint16>(A[reg].w);
                    bus->write<uint8>(A[reg].w, read + value);
                }
                else if (operationSize == OperationSize::Long) {
                    const auto read = bus->read<uint32>(A[reg].l);
                    bus->write<uint8>(A[reg].l, read + value);
                }
                break;
            }
            case AddressingMode::AddressPostIncrement: {
                ss << "(A" << static_cast<int>(reg) << ")+";
                if (operationSize == OperationSize::Byte) {
                    const auto read = bus->read<uint8>(A[reg].b);
                    bus->write<uint8>(A[reg].b, read + value);
                    A[reg].b++;
                }
                else if (operationSize == OperationSize::Word) {
                    const auto read = bus->read<uint16>(A[reg].w);
                    bus->write<uint8>(A[reg].w, read + value);
                    A[reg].w += 2;
                }
                else if (operationSize == OperationSize::Long) {
                    const auto read = bus->read<uint32>(A[reg].l);
                    bus->write<uint8>(A[reg].l, read + value);
                    A[reg].l += 4;
                }
                break;
            }
            case AddressingMode::AddressPreDecrement: {
                ss << "-(A" << static_cast<int>(reg) << ")";
                if (operationSize == OperationSize::Byte) {
                    A[reg].b--;
                    const auto read = bus->read<uint8>(A[reg].b);
                    bus->write<uint8>(A[reg].b, read + value);
                }
                else if (operationSize == OperationSize::Word) {
                    A[reg].w -=2;
                    const auto read = bus->read<uint16>(A[reg].w);
                    bus->write<uint8>(A[reg].w, read + value);
                }
                else if (operationSize == OperationSize::Long) {
                    A[reg].l -= 4;
                    const auto read = bus->read<uint32>(A[reg].l);
                    bus->write<uint8>(A[reg].l, read + value);
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
            }
        }
    };

    OperationSize operationSize = OperationSize::UnknownSize;
    if (bits8to9 == 0) {
        operationSize = OperationSize::Byte;
        preStream << ".b";
    }
    else if (bits8to9 == 1) {
        operationSize = OperationSize::Word;
        preStream << ".w";
    }
    else if (bits8to9 == 2) {
        operationSize = OperationSize::Long;
        preStream << ".l";
    }

    const auto reg = static_cast<int>(bits13to15);
    const auto value = bus->read<uint32>(PC);
    PC += 4;
    srcStream << std::hex << "#$" << value;

    AddressingMode addressingMode = getAddressingMode(bits10to12, bits13to15);

    write(addressingMode, operationSize, reg, value, dstStream);
    
    postStream << '\n';

    std::cout << preStream.str() << " " <<srcStream.str() << ", " << dstStream.str() << postStream.str();
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
    std::stringstream preStream, srcStream, dstStream, postStream;
    preStream << "move";

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

    auto read = [this](AddressingMode addressingMode, OperationSize operationSize, int reg, std::stringstream & ss) -> uint32 {
        uint32 value = 0;
        ss << std::hex;
        switch (addressingMode) {
            case AddressingMode::DataRegister: {
                ss << "D" << static_cast<int>(reg);
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
                ss << "A" << static_cast<int>(reg);
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
                ss << "(A" << static_cast<int>(reg) << ")";
                if (operationSize == OperationSize::Byte) {
                    value = bus->read<uint8>(A[reg].l); // addresses are longs
                }
                else if (operationSize == OperationSize::Word) {
                    value = bus->read<uint16>(A[reg].l); // expansion
                }
                else if (operationSize == OperationSize::Long) {
                    value = bus->read<uint32>(A[reg].l);
                }
                break;
            }
            case AddressingMode::AddressPostIncrement: {
                ss << "(A" << static_cast<int>(reg) << ")+";
                if (operationSize == OperationSize::Byte) {
                    value = bus->read<uint8>(A[reg].l); // addresses are longs
                    A[reg].b++;
                }
                else if (operationSize == OperationSize::Word) {
                    value = bus->read<uint16>(A[reg].l); // expansion
                    A[reg].w += 2;
                }
                else if (operationSize == OperationSize::Long) {
                    value = bus->read<uint32>(A[reg].l);
                    A[reg].l += 4;
                }
                break;
            }
            case AddressingMode::AddressPreDecrement: {
                ss << "-(A" << static_cast<int>(reg) << ")";
                if (operationSize == OperationSize::Byte) {
                    A[reg].b--;
                    value = bus->read<uint8>(A[reg].l); // addresses are longs
                }
                else if (operationSize == OperationSize::Word) {
                    A[reg].w -= 2;
                    value = bus->read<uint16>(A[reg].l); // expansion
                }
                else if (operationSize == OperationSize::Long) {
                    A[reg].l -= 4;
                    value = bus->read<uint32>(A[reg].l);
                }
                break;
            }
            case AddressingMode::AddressDisplacement: {
                ss << "displacement(A[" << static_cast<int>(reg) << "])";
                break;
            }
            case AddressingMode::AddressIndex: {
                ss << "index(A[" << static_cast<int>(reg) << "])";
                break;
            }
            case AddressingMode::ProgramCounterDisplacement: {
                ss << "PC.displacement";
                break;
            }
            case AddressingMode::ProgramCounterIndex: {
                ss << "PC.index";
                break;
            }
            case AddressingMode::AbsoluteShort: {
                const uint16 read = bus->read<uint16>(PC); // might be read<uint32>?
                PC += 2;
                value = bus->read<uint16>(read);
                ss << "($" << static_cast<int>(read) << ")";
                break;
            }
            case AddressingMode::AbsoluteLong: {
                const uint32 read = bus->read<uint32>(PC);
                PC += 4;
                if (operationSize == OperationSize::Byte) {
                    value = bus->read<uint8>(read);
                }
                else if (operationSize == OperationSize::Word) {
                    value = bus->read<uint16>(read);
                }
                else if (operationSize == OperationSize::Long) {
                    value = bus->read<uint32>(read);
                }
                ss << "($" << static_cast<int>(read) << ")";
                break;
            }
            case AddressingMode::Immediate: {
                if (operationSize == OperationSize::Byte) {
                    throw std::runtime_error("Invalid intermediate read!");
                }
                else if (operationSize == OperationSize::Word) {
                    value = bus->read<uint16>(PC);
                    PC += 2;
                }
                else if (operationSize == OperationSize::Long) {
                    value = bus->read<uint32>(PC);
                    PC += 4;
                }
                ss << "#$" << static_cast<int>(value);
                break;
            }
            default: {
                throw std::exception("Unknown addressing mode!");
                break;
            }
        }
        return value;
    };

    auto write = [this](AddressingMode addressingMode, OperationSize operationSize, uint8 reg, uint32 value, std::stringstream & ss) {
        ss << std::hex;
        switch (addressingMode) {
            case AddressingMode::DataRegister: {
                ss << "D" << static_cast<int>(reg);
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
                ss << "A" << static_cast<int>(reg);
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
                ss << "(A" << static_cast<int>(reg) << ")";
                if (operationSize == OperationSize::Byte) {
                    bus->write<uint8>(A[reg].b, value);
                }
                else if (operationSize == OperationSize::Word) {
                    bus->write<uint16>(A[reg].w, value);
                }
                else if (operationSize == OperationSize::Long) {
                    bus->write<uint32>(A[reg].l, value);
                }
                break;
            }
            case AddressingMode::AddressPostIncrement: {
                ss << "(A" << static_cast<int>(reg) << ")+";
                if (operationSize == OperationSize::Byte) {
                    bus->write<uint8>(A[reg].b, value);
                    A[reg].b++;
                }
                else if (operationSize == OperationSize::Word) {
                    bus->write<uint16>(A[reg].w, value);
                    A[reg].w += 2;
                }
                else if (operationSize == OperationSize::Long) {
                    bus->write<uint32>(A[reg].l, value);
                    A[reg].l += 4;
                }
                break;
            }
            case AddressingMode::AddressPreDecrement: {
                ss << "-(A" << static_cast<int>(reg) << ")";
                if (operationSize == OperationSize::Byte) {
                    A[reg].b--;
                    bus->write<uint8>(A[reg].b, value);
                }
                else if (operationSize == OperationSize::Word) {
                    A[reg].w -= 2;
                    bus->write<uint16>(A[reg].w, value);
                }
                else if (operationSize == OperationSize::Long) {
                    A[reg].l -= 4;
                    bus->write<uint32>(A[reg].l, value);
                }
                break;
            }
            case AddressingMode::AddressDisplacement: {
                ss << "displacement(A" << static_cast<int>(reg) << ") ";
                break;
            }
            case AddressingMode::AddressIndex: {
                ss << "index(A" << static_cast<int>(reg) << ") ";
                break;
            }
            case AddressingMode::ProgramCounterDisplacement: {
                ss << "PC.displacement ";
                break;
            }
            case AddressingMode::ProgramCounterIndex: {
                ss << "index(A" << static_cast<int>(reg) << ") ";
                break;
            }
            case AddressingMode::AbsoluteShort: {
                const auto address = bus->read<uint16>(PC);
                ss << "($" << static_cast<int>(address) << ")";
                PC += 2;
                if (operationSize == OperationSize::Byte) {
                    bus->write<uint8>(address, value);
                }
                else if (operationSize == OperationSize::Word) {
                    bus->write<uint16>(address, value);
                }
                else if (operationSize == OperationSize::Long) {
                    bus->write<uint32>(address, value);
                }
                break;
            }
            case AddressingMode::AbsoluteLong: {
                const auto address = bus->read<uint32>(PC);
                ss << "($" << static_cast<int>(address) << ")";
                PC += 4;
                if (operationSize == OperationSize::Byte) {
                    bus->write<uint8>(address, value);
                }
                else if (operationSize == OperationSize::Word) {
                    bus->write<uint16>(address, value);
                }
                else if (operationSize == OperationSize::Long) {
                    bus->write<uint32>(address, value);
                }
                break;
            }
            case AddressingMode::Immediate: {
                throw std::runtime_error("Immediate mode write!");
                /*
                const auto address = bus->read<uint32>(PC);
                PC += 4;
                if (operationSize == OperationSize::Byte) {
                    bus->write<uint8>(address, value);
                }
                else if (operationSize == OperationSize::Word) {
                    bus->write<uint16>(address, value);
                }
                else if (operationSize == OperationSize::Long) {
                    bus->write<uint32>(address, value);
                }
                break;
                */
            }
            default: {
                throw std::exception("Unknown addressing mode!");
            }
        }
    };

    OperationSize operationSize = OperationSize::UnknownSize;
    if (bits2to3 == 1) {
        operationSize = OperationSize::Byte;
        preStream << ".b ";
    }
    else if (bits2to3 == 2) {
        operationSize = OperationSize::Long;
        preStream << ".l ";
    }
    else if (bits2to3 == 3) {
        operationSize = OperationSize::Word;
        preStream << ".w ";
    }

    AddressingMode srcAddressingMode = getAddressingMode(bits10to12, bits13to15);
    AddressingMode dstAddressingMode = getAddressingMode(bits7to9, bits4to6);
    
    uint32 srcValue = read(srcAddressingMode, operationSize, bits13to15, srcStream);
    write(dstAddressingMode, operationSize, bits4to6, srcValue, dstStream);

    postStream << '\n';

    std::cout << preStream.str() << srcStream.str()<< ", " << dstStream.str() << postStream.str();

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
    const uint32 value = bus->read<uint32>(PC);
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
            const uint32 value = bus->read<uint32>(PC);
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
    const uint32 value = bus->read<uint32>(PC);
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
    const uint16 value = bus->read<uint16>(PC);
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
    std::stringstream preStream, dstStream, postStream;
    preStream << "b";
    const int condition = static_cast<int>(bits4to7);
    short displacement{ 0 };
    if (bits8to15 == 0) { // optional 16-bit displacement
        displacement = static_cast<short>(bus->read<uint16>(PC));
        PC += 2;
    }
    else { // op code contained 8-bit displacement
        displacement = static_cast<short>(bits8to15);
    }

    switch (condition) {
    case 0b0000: {
        preStream << "true";
        break;
    }
    case 0b0001: {
        preStream << "false";
        break;
    }
    case 0b0010: {
        preStream << "higher";
        break;
    }
    case 0b0011: {
        preStream << "lowerOrSame";
        break;
    }
    case 0b0100: {
        preStream << "CarryClear";
        break;
    }
    case 0b0101: {
        preStream << "CarrySet";
        break;
    }
    case 0b0110: {
        preStream << "ne";
        if (SR[StatusRegister::Z] != 1) {
            PC += displacement - 2;
        }
        break;
    }
    case 0b0111: {
        preStream << "eq";
        if (SR[StatusRegister::Z] == 1) {
            PC += displacement - 2;
        }
        break;
    }
    case 0b1000: {
        preStream << "OverflowClear";
        break;
    }
    case 0b1001: {
        preStream << "OverflowSet";
        break;
    }
    case 0b1010: {
        preStream << "Plus";
        break;
    }
    case 0b1011: {
        preStream << "Minus";
        break;
    }
    case 0b1100: {
        preStream << "GreaterOrEqual";
        break;
    }
    case 0b1101: {
        preStream << "LessThan";
        break;
    }
    case 0b1110: {
        preStream << "GreaterThan";
        break;
    }
    case 0b1111: {
        preStream << "LessOrEqual";
        break;
    }
    }

    dstStream << std::hex << static_cast<int>(displacement);
    postStream << '\n';
    std::cout << preStream.str() << " " << dstStream.str() << " " << postStream.str();
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

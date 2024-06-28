#pragma once

#include "defs.hpp"
#include "Cpu68000.hpp"
#include "VDP.hpp"
#include "Graphics.hpp"

#include <iostream>
#include <vector>

enum class BusItem {
    Rom,
    Ram
};

class Bus {
public:
    Bus();
    ~Bus();

    void loop();

    void clearMemory(BusItem busItem);
    void printMemory(BusItem busItem, unsigned begin, unsigned end);
    void printHeader();

    template<typename T, typename halfT>
    std::pair<halfT, halfT> split(T data) {
        const auto lsb = static_cast<halfT>(data);
        const auto msb = static_cast<halfT>(data >> (8*sizeof(halfT)));
        return { msb, lsb };
    }

    template<typename T>
    constexpr OperationSize getOperationSize() {
        if constexpr (sizeof(T) == 1) {
            return OperationSize::Byte;
        }
        else if constexpr (sizeof(T) == 2) {
            return OperationSize::Word;
        }
        else if constexpr (sizeof(T) == 4) {
            return OperationSize::Long;
        }
        else {
            return OperationSize::UnknownSize;
        }
    }

    template<typename DataType>
    DataType endianSwap(DataType data) {
        OperationSize operationSize = getOperationSize<DataType>();
        if (operationSize == OperationSize::Word) {
            const DataType lsb = (data & 0xFF00) >> 8;
            const DataType msb = data << 8;
            return msb | lsb;
        }
        else if (operationSize == OperationSize::Long) {
            const DataType byte4 = (data & 0xFF000000) >> 24;
            const DataType byte3 = (data & 0x00FF0000) >> 8;
            const DataType byte2 = (data & 0x0000FF00) << 8;
            const DataType byte1 = (data & 0x000000FF) << 24;
            return byte1 | byte2 | byte3 | byte4;
        }
        else {
            return data;
        }
    }

    template<typename DataType>
    DataType read(uint32 addr) {
        //constexpr OperationSize operationSize = getOperationSize<DataType>();

        if (addr < 0x400000) {
            const auto * mapPtr = reinterpret_cast<DataType*>(map.get());
            const auto data = mapPtr[addr / sizeof(DataType)];
            return endianSwap<DataType>(data);
        }
        else if (addr < 0x800000) {
            throw std::runtime_error("Sega CD and 32X space read!");
        }
        else if (addr < 0xA00000) {
            throw std::runtime_error("32X ? space read!");
        }
        else if (addr < 0xA10000) {
            std::cout << "Z80 space read";
            return 0;
        }
        else if (addr < 0xA10020) {
            return static_cast<DataType>(hasTmss); // No TMSS
        }
        else if (addr < 0xFF0000) {
            if (addr == 0xC00004) {
                return static_cast<DataType>(vdp.getStatus());
            }
            else {
                return 0;
            }
        }
        else if (addr < 0x1000000) {
            std::cout << "RAM read ";
            return 0;
        }
        else {
            throw std::runtime_error("Read above address space!");
        }
    }

    template<typename DataType>
    void write(uint32 addr, DataType data) {
        if (addr < 0x400000) {
            throw std::runtime_error("ROM write!");
        }
        else if (addr < 0x800000) {
            throw std::runtime_error("Sega CD and 32X space write!");
        }
        else if (addr < 0xA00000) {
            throw std::runtime_error("32X ? space write!");
        }
        else if (addr < 0xA10000) {
            std::cout << "Z80 space write";
            return;
        }
        else if (addr < 0xA10020) {
            std::cout << "IO write";
            return;
        }
        else if (addr < 0xFF0000) {
            if (addr == 0xA14000) {
                tmss = data;
            }
            else if (addr == 0xC00004) {
                vdp.processData(data);
            }
            else if (addr == 0xC00000) {
                //vdp.processCtrl(data);
            }
        }
        else if (addr < 0x1000000) {
            std::cout << "RAM write";
        }
        else {
            std::cout << "Error - write above address space!";
        }

        map[addr] = data;
    }

private:
    Cpu68000 cpu68000;
    VDP vdp;
    Graphics graphics;

    uint32 tmss;
    bool hasTmss;

    std::unique_ptr<uint8[]> map = nullptr;
};

// ----------------------------------------------------------------------------
//    68k Memory map
// ----------------------------------------------------------------------------
//
// $000000     $3FFFFF     Cartridge ROM / RAM
// $400000     $7FFFFF     Reserved(used by the Sega CD and 32x)
// $800000     $9FFFFF     Reserved(used by the 32x ? )
// $A00000     $A0FFFF     Z80 addressing space
// $A00000     $A01FFF     Sound Ram; Halt Z80 before 68k access A00000 to A0FFFF
// $A02000     $A03FFF     Reserved
// $A04000                 YM2612 A0
// $A04001                 YM2612 D0
// $A04002                 YM2612 A1
// $A04003                 YM2612 D1
// $A04000     $A05FFF     Sound Chip
// $A06000                 Bank Register
// $A06000     $A07F10     Misc
// $A07F11                 PSG 76489
// $A07F12     $A07FFF     Misc
// $A08000     $A0FFFF     68000 Bank
// $A10000     $A10001     Version register (read - only word - long)
// $A10002     $A10003     Controller 1 data
// $A10004     $A10005     Controller 2 data
// $A10006     $A10007     Expansion port data
// $A10008     $A10009     Controller 1 control
// $A1000A     $A1000B     Controller 2 control
// $A1000C     $A1000D     Expansion port control
// $A1000E     $A1000F     Controller 1 serial transmit
// $A10010     $A10011     Controller 1 serial receive
// $A10012     $A10013     Controller 1 serial control
// $A10014     $A10015     Controller 2 serial transmit
// $A10016     $A10017     Controller 2 serial receive
// $A10018     $A10019     Controller 2 serial control
// $A1001A     $A1001B     Expansion port serial transmit
// $A1001C     $A1001D     Expansion port serial receive
// $A1001E     $A1001F     Expansion port serial control
// $A10020     $A10FFF     Reserved
// $A11000                 Memory mode register
// $A11002     $A110FF     Reserved
// $A11100     $A11101     Z80 bus request
// $A11102     $A111FF     Reserved
// $A11200     $A11201     Z80 reset
// $A11202     $A13FFF     Reserved
// $A14000     $A14003     TMSS register
// $A14004     $BFFFFF     Reserved
// $C00000                 VDP Data Port
// $C00002                 VDP Data Port(Mirror)
// $C00004                 VDP Control Port
// $C00006                 VDP Control Port(Mirror)
// $C00008                 H / V Counter
// $C0000A                 H / V Counter(Mirror)
// $C0000C                 H / V Counter(Mirror)
// $C0000E                 H / V Counter(Mirror)
// $C00011                 SN76489 PSG
// $C00013                 SN76489 PSG(Mirror)
// $C00015                 SN76489 PSG(Mirror)
// $C00017                 SN76489 PSG(Mirror)
// $C0001C                 Disable / Debug register
// $C0001E                 Disable / Debug register (Mirror)
// $C0001E     $FEFFFF     Reserved
// $FF0000     $FFFFFF     68000 RAM


// ---------------------------------
//    z80 Memory map
// ---------------------------------
//
// $0000     $1FFF     Sound Ram
// $2000     $3FFF     Reserved
// $4000               YM2612 A0
// $4001               YM2612 D0
// $4002               YM2612 A1
// $4003               YM2612 D1
// $4000     $5FFF     Sound Chip
// $6000               Bank Register
// $6000     $7F10     Misc
// $7F11               PSG 76489
// $7F12     $7FFF     Misc
// $8000     $FFFF     68000 Bank
#pragma once

#include "Cpu68000.hpp"
#include "VDP.hpp"
#include "Graphics.hpp"

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

    uint8 readByte(uint32 addr);
    uint16 readWord(uint32 addr);
    uint32 readLong(uint32 addr);

    void writeByte(uint32 addr, uint8 data);
    void writeWord(uint32 addr, uint16 data);
    void writeLong(uint32 addr, uint32 data);

private:
    Cpu68000 cpu68000;
    VDP vdp;
    Graphics graphics;

    std::vector<uint8> map;
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
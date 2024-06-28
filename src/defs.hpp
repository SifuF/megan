#pragma once

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned uint32;

namespace Endian {
    static uint16 swapWord(uint16 data) {
        const uint16 lsb = (data & 0xFF00) >> 8;
        const uint16 msb = data << 8;
        return msb | lsb;
    }

    static uint32 swapLong(uint32 data) {
        const uint32 byte4 = (data & 0xFF000000) >> 24;
        const uint32 byte3 = (data & 0x00FF0000) >> 8;
        const uint32 byte2 = (data & 0x0000FF00) << 8;
        const uint32 byte1 = (data & 0x000000FF) << 24;
        return byte1 | byte2 | byte3 | byte4;
    }
}

enum OperationSize {
    Byte = 0,
    Word,
    Long,
    UnknownSize
};
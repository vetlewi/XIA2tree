//
// Created by Vetle Wegner Ingeberg on 20/04/2026.
//


#include <iostream>
#include <iomanip>
#include <ostream>
#include <vector>

#include <cstdint>

#include "xiaformat.h"
#include "MemoryMap.h"

struct trace_word {
    uint16_t sample0;
    uint16_t sample1;
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: FileInspector filename" << std::endl;
    }

    // Open and memory map the file
    IO::MemoryMap map(argv[1]);
    const auto *begin = map.GetPtr<uint32_t>();
    const auto *end = begin + map.GetSize<uint32_t>();
    const auto *pos = begin;

    std::cout << "File size (bytes): " << map.GetSize<char>() << std::endl;
    std::cout << "File size (32-bit words): " << map.GetSize<uint32_t>() << std::endl;

    int event_size = 0;
    int event_no = 0;
    while ( pos < end ) {
        auto header = reinterpret_cast<const XIA_base_t*>(pos);
        auto header2 = reinterpret_cast<const XIA_event_4_t *>(pos);
        auto trace_begin = reinterpret_cast<const uint16_t *>(pos+4);
        int size_left = end - pos;
        auto trace = getTrace(header);
        std::vector<uint16_t> trace2;
        for ( int i = 0 ; i < header->traceLen ; ++i) {
            trace2.push_back(*(trace_begin+i));
        }

        std::cout << "------------- Event #" << event_size << " -------------" << std::endl;
        std::cout << "Word 0: 0x" << std::hex << std::setw(8) << std::setfill('0') << pos[0] << std::dec;
        std::cout << "(" << pos[0] << ")" << std::endl;
        std::cout << "\tCrate #: " << header->crateID << " Slot #: " << header->slotID;
        std::cout << " Channel #: " << header->chanID << std::endl;
        std::cout << "\tFinish code: " << std::boolalpha << header->finishCode << std::endl;
        std::cout << "\tHeader len: " << header->headerLen << std::endl;
        std::cout << "\tEvent len: " << header->eventLen << std::endl;
        std::cout << "Word 1: 0x" << std::hex << std::setw(8) << std::setfill('0') << pos[1] << std::dec;
        std::cout << "(" << pos[1] << ")" << std::endl;
        std::cout << "\tEvent time low: " << header->event_time_low << std::endl;
        std::cout << "Word 2: 0x" << std::hex << std::setw(8) << std::setfill('0') << pos[2] << std::dec;
        std::cout << "(" << pos[2] << ")" << std::endl;
        std::cout << "\tEvent time high: " << header->event_time_high << std::endl;
        std::cout << "\tCFD: " << header->cfd_result << std::endl;
        std::cout << "Word 3: 0x" << std::hex << std::setw(8) << std::setfill('0') << pos[3] << std::dec;
        std::cout << "(" << pos[3] << ")" << std::endl;
        std::cout << "\tTrace-out-of-range: " << std::boolalpha << header->traceOutOfRange << std::endl;
        std::cout << "\tTrace length: " << header->traceLen << std::endl;
        std::cout << "\tEvent energy: " << header->eventEnergy << std::endl;
        for ( int i = 0 ; i < header->eventLen - header->headerLen ; ++i ) {
            std::cout << "Word " << i << ": 0x" << std::hex << std::setw(8) << pos[i+4] << std::dec;
            std::cout << "(" << pos[i+4] << ")" << std::endl;
            uint32_t word = pos[i+4];
            uint16_t upper = static_cast<uint16_t>((word >> 16) & 0xFFFF);
            uint16_t lower = static_cast<uint16_t>(word & 0xFFFF);
            std::cout << "\tSample# " << 2*i << ": " << lower << std::endl;
            std::cout << "\tSample# " << 2*i+1 << ": " << upper << std::endl;
        }
        return 0;


        for ( int i = 0 ; i < header->eventLen ; ++i ) {
            std::cout
        << "[" << i << "] "
        << "0x"
        << std::hex
        << std::setw(8)
        << std::setfill('0')
        << pos[i]
        << std::endl;
        }

        if (header->eventLen > size_left) {
            std::cerr << "Event size overflows the file, number of words until end of file: " << size_left << std::endl;
        }
        pos += header->eventLen;
        ++event_size;
    }
    std::cout << "Number of events: " << event_size << std::endl;
    pos = begin;

    return 0;
}

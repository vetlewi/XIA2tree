//
// Created by Vetle Wegner Ingeberg on 20/04/2026.
//


#include <iostream>
#include <ostream>
#include <vector>

#include <cstdint>

#include "xiaformat.h"
#include "MemoryMap.h"

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
    while ( pos < end ) {
        auto header = reinterpret_cast<const XIA_base_t*>(pos);
        int size_left = end - pos;
        auto trace = getTrace(header);
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

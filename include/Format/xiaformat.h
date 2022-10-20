//
// Created by Vetle Wegner Ingeberg on 06/04/2022.
//

#ifndef XIAFORMAT_H
#define XIAFORMAT_H

struct XIA_address_t {
    // First 32-bit word
    unsigned short chanID : 4;
    unsigned short slotID : 4;
    unsigned short crateID : 4;

    operator unsigned short(){ return *reinterpret_cast<unsigned short *>(this); }
};

struct XIA_base_t {
    // First 32-bit word
    unsigned chanID : 4;
    unsigned slotID : 4;
    unsigned crateID : 4;

    unsigned headerLen : 5;
    unsigned eventLen : 14;
    bool finishCode : 1;

    // Second 32-bit word
    unsigned event_time_low : 32;

    // Third 32-bit word
    unsigned event_time_high : 16;
    unsigned cfd_result : 16;

    // Forth 32-bit word
    unsigned eventEnergy : 16;
    unsigned traceLen : 15;
    bool traceOutOfRange : 1;

    inline unsigned short index() const { return (crateID << 8) + (slotID << 4) + chanID; }
    inline int64_t timestamp() const {
        int64_t timestamp = event_time_high;
        timestamp <<= 32;
        timestamp |= event_time_low;
        return timestamp;
    }
};

struct XIA_esums_t {
    unsigned trailing : 32;
    unsigned leading : 32;
    unsigned gap : 32;
    unsigned baseline : 32;
};

struct XIA_qdcsums_t {
    unsigned qdc[8];
};

struct XIA_external_timestamp_t {
    unsigned timestamp_low : 32;
    unsigned timestamp_high : 16;
    unsigned unused : 16;
};


#endif // XIAFORMAT_H

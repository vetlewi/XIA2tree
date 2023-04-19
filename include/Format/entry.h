//
// Created by Vetle Wegner Ingeberg on 06/04/2022.
//

#ifndef ENTRY_H
#define ENTRY_H

#include "PhysicalParam/DetectorTypes.h"

struct Entry_t {

    DetectorType type;
    unsigned short detectorID; // I.e. detector number.

    unsigned short adcvalue;
    unsigned short cfdvalue;
    long long timestamp;
    bool finishflag; // Pile-up flag

    // Derived values
    double energy;
    double cfdcorr;
    bool cfdfail;
};

#endif // ENTRY_H

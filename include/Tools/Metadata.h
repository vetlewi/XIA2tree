//
// Created by Vetle Wegner Ingeberg on 24/03/2026.
//

#ifndef METADATA_H
#define METADATA_H

/*!
 * Functionality to write all metadata to a root file.
 * This includes:
 *   - Start time
 *   - End time
 *   - Date
 *   - Input file(s) (names) + shasum256
 *   - CPU type
 *   - System memory
 *   - OS name/version
 *   - Software name
 *   - Software version + git tag if possible
 *   - Build date
 *   - Build time
 *   - Build type
 *   - Compiler version
 *   - Compiler type
 *   - C++ version
 *   - Architecture
 *   - Command line options
 *   - ROOT version
 *   - Calibration data
 *   - Calibration coefficients
 *   - User sort code (Preferably embed the code?)
 *   - Number of entries in the raw files
 *   - Number of entries for each detector type
 *   - Number of rejected entries (i.e. mod/chan not mapped to a detector)
 */

#include <string>

namespace Metadata {
    struct SystemMetadata_t {
        std::string cpuVendor;
        std::string cpuName;
    };

    struct RunMetadata_t {
        std::chrono::time_point<std::chrono::system_clock> starTime;
        std::chrono::time_point<std::chrono::system_clock> endTime;

        size_t number;

    };
}

void tst_hwinfo();

#endif // METADATA_H
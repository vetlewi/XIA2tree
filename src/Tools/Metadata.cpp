//
// Created by Vetle Wegner Ingeberg on 24/03/2026.
//

#include "Tools/Metadata.h"

#include <hwinfo/hwinfo.h>
#include <hwinfo/utils/unit.h>

#include <iostream>
using namespace hwinfo::unit;

void tst_hwinfo() {

    auto cpus = hwinfo::getAllCPUs();
    for (auto& cpu : cpus) {
        std::cout << "CPU ID: " << cpu.id() << std::endl;
        std::cout << "CPU vendor: " << cpu.vendor() << std::endl;
        std::cout << "CPU name: " << cpu.modelName() << std::endl;
        std::cout << "CPU number of phys. cores: " << cpu.numPhysicalCores() << std::endl;
        std::cout << "CPU number of log. cores: " << cpu.numLogicalCores() << std::endl;
        std::cout << "max frequency:" << cpu.maxClockSpeed_MHz() << std::endl;
        std::cout << "regular frequency:" << cpu.regularClockSpeed_MHz() << std::endl;
        std::cout << "cache size [KiB]:\t" << unit_prefix_to(cpu.L1CacheSize_Bytes(), IECPrefix::KIBI) << std::endl;
        std::cout << "\t\t" << unit_prefix_to(cpu.L2CacheSize_Bytes(), IECPrefix::KIBI) << std::endl;
        std::cout << "\t\t" << unit_prefix_to(cpu.L3CacheSize_Bytes(), IECPrefix::GIBI) << std::endl;
        std::cout << "CPU flags: " << std::endl;
        for (const auto& flag : cpu.flags()){
            std::cout << "\t\t" << flag << std::endl;
        }
    }
}
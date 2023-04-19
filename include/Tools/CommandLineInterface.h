#ifndef COMMAND_LINE_INTERFACE_H
#define COMMAND_LINE_INTERFACE_H

#include <vector>
#include <string>
#include <optional>

#include <PhysicalParam/DetectorTypes.h>

namespace CLI {

    enum veto_action {
        ignore,
        keep,
        remove
    };

    enum sort_type {
        coincidence,
        gap,
        time
    };

    struct Options
    {
        // Required arguments
        std::optional<std::vector<std::string>> input;
        std::optional<std::string> output;

        // Optional arguments
        std::optional<std::string> CalibrationFile;
        std::optional<std::string> RangeFile;
        std::optional<std::string> userSort;
        std::optional<double> coincidenceTime = 1500;
        std::optional<double> SplitTime = 1500;
        std::optional<bool> tree = false;
        std::optional<sort_type> sortType = sort_type::coincidence;
        std::optional<DetectorType> Trigger = DetectorType::eDet;
    };

    Options ParseCLA(const int &argc, char *argv[]);

}

std::ostream &operator<<(std::ostream &os, const CLI::Options &opt);

#endif // COMMAND_LINE_INTERFACE_H
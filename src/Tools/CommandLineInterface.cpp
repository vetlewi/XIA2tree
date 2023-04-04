#include "CommandLineInterface.h"

#include "Calibration.h"

#include <structopt/app.hpp>
#include <structopt/third_party/magic_enum/magic_enum.hpp>


using namespace CLI;

STRUCTOPT(Options, input, output, CalibrationFile, RangeFile, userSort, coincidenceTime,
          SplitTime, tree, sortType, Trigger);

std::ostream &operator<<(std::ostream &os, const Options &opt)
{
    os << "Sorting with following options:\n";
    os << "\tInput file(s):\n";
    for ( auto &file : opt.input.value() ){
        os << "\t\t" << file << "\n";
    }
    os << "\tOutput file: " << opt.output.value() << "\n";
    os << "\tCalibration file: " << opt.CalibrationFile.value_or("") << "\n";
    os << "\tRange file: " << opt.RangeFile.value_or("") << "\n";
    os << "\tCustom user sorting routine: " << opt.userSort.value_or("") << "\n";
    os << "\tCoincidence time: " << opt.coincidenceTime.value() << " ns\n";
    os << "\tSplit time: " << opt.SplitTime.value() << " ns\n";

    os << "\tBuild tree: " << std::boolalpha << opt.tree.value() << "\n";
    os << "\tSort type: " << magic_enum::enum_name(opt.sortType.value()) << "\n";
    os << "\tTrigger: " << magic_enum::enum_name(opt.Trigger.value()) << "\n";
    return os;
}

Options CLI::ParseCLA(const int &argc, char *argv[])
{
    Options options;
    try {
        structopt::app app("XIA2tree", "0.9.9");
        structopt::details::visitor vis("XIA2tree", "0.9.9");
        visit_struct::for_each(options, vis);
        options = app.parse<Options>(argc, argv);
        if ( !options.input.has_value() ){
            throw structopt::exception("Input(s) missing", vis);
        }
        if ( !options.output.has_value() ){
            throw structopt::exception("Output missing", vis);
        }
    } catch ( const structopt::exception &e ){
        std::cerr << e.what() << "\n";
        std::cout << e.help();
        throw e;
    }

    try {
        auto &files = options.input.value();
        std::sort(files.begin(), files.end());
        std::rotate(files.rbegin(), files.rbegin()+1, files.rend());
    } catch ( const std::exception &e ){
        std::cerr << e.what() << std::endl;
        throw e;
    }

    std::cout << options << std::endl;

    return options;
}
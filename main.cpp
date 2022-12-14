#include <iostream>
#include <fstream>
#include <thread>
#include <signal.h>

#include "PhysicalParam/Calibration.h"

#include "histogram/RootWriter.h"

#include "Tasks/Tasks.h"
#include "Tasks/XIAReader.h"
#include "Tasks/Calibrator.h"
#include "Tasks/Buffer.h"
#include "Tasks/Splitter.h"
#include "Tasks/Trigger.h"
//#include "Tasks/Sort.h"
#include "Tasks/MTSort.h"

#include "Tools/CommandLineInterface.h"
#include "Tools/ProgressUI.h"
#include "ThreadPool.hpp"

int main(int argc, char *argv[])
{
    CLI::Options options;
    try {
        options = CLI::ParseCLA(argc, argv);
    } catch ( std::exception &e ){
        return 1; // Error
    }
    std::ifstream cal_file;
    try {
        cal_file = std::ifstream(options.CalibrationFile.value());
    } catch ( std::exception &e ){
        // Pass, do nothing.
    }
    Calibration cal(cal_file);
    ProgressUI progress;

    Task::XIAReader reader(options.input.value(), &progress);
    Task::Calibrator calibrator(cal, reader.GetQueue());
    Task::Buffer buffer(calibrator.GetQueue());
    Task::Splitter splitter(buffer.GetQueue(), options.SplitTime.value());
    //Task::Trigger trigger(splitter.GetQueue(), options.coincidenceTime.value(),
    //                      options.Trigger.value(), (options.sortType.value() == CLI::sort_type::time));

    Task::Triggers triggers(splitter.GetQueue(), options.coincidenceTime.value(),
                            options.Trigger.value(), options.sortType.value());

    const char *user_sort = nullptr;
    if ( options.userSort.has_value() )
        user_sort = options.userSort->c_str();
    Task::Sorters sorters(triggers.GetQueue(), user_sort);
    //Task::Sort sort(trigger.GetQueue(), user_sort);
    /*Task::Sort sorters[] = {Task::Sort(triggers.GetQueue(), user_sort),
                            Task::Sort(triggers.GetQueue(), user_sort),
                            Task::Sort(triggers.GetQueue(), user_sort),
                            Task::Sort(triggers.GetQueue(), user_sort)};*/


    ThreadPool<std::thread> pool;
    pool.AddTask(&reader);
    pool.AddTask(&calibrator);
    pool.AddTask(&buffer);
    pool.AddTask(&splitter);
    pool.AddTask(triggers.GetNewTrigger());
    pool.AddTask(triggers.GetNewTrigger());

    // We use additional threads for the final step if there is more than one file.
    // This will speed up the work considerably.
    /*if ( options.input.value().size() == 1 ) {
        pool.AddTask(&sort);
    } else {
        for (auto &_sort: sorters) {
            pool.AddTask(&_sort);
        }
    }*/
    /*for (auto &_sort : sorters) {
        pool.AddTask(&_sort);
    }*/
    for ( int i = 0 ; i < 4 ; ++i ){
        pool.AddTask(sorters.GetNewSorter());
    }
    //pool.AddTask(sorters.GetNewSorter());

    try {
        pool.Wait();
    } catch ( const std::exception &ex ){
        std::cerr << "Got exception: " << ex.what() << std::endl;
    }

    // Write to file
    //Histograms &hm = sorters[0].GetHistograms();
    Histograms &hm = sorters.GetHistograms();
    /*int i = 0;
    for ( auto &_sort : sorters ){
        if ( i++ == 0 )
            continue;
        hm.Merge(_sort.GetHistograms());
    }*/
    RootWriter::Write(hm, options.output.value());
    return 0;
}

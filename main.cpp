#include <iostream>
#include <fstream>
#include <thread>
#include <signal.h>

#include <ROOT/TBufferMerger.hxx>

#include "PhysicalParam/ConfigManager.h"
#include "PhysicalParam/ParticleRange.h"
#include "histogram/RootWriter.h"

#include "Tasks/Tasks.h"
#include "Tasks/XIAReader.h"
#include "Tasks/Calibrator.h"
#include "Tasks/Buffer.h"
#include "Tasks/Splitter.h"
#include "Tasks/Trigger.h"
#include "Tasks/MTSort.h"

#include "Tools/CommandLineInterface.h"
#include "Tools/ProgressUI.h"
#include "ThreadPool.hpp"

#include <TROOT.h>
#include <TFileMerger.h>

std::vector<std::string> RunSort(const CLI::Options &options)
{
    ProgressUI progress;

    std::ifstream cal_file;
    try {
        cal_file = std::ifstream(options.CalibrationFile.value());
    } catch ( std::exception &e ){
        // Pass, do nothing.
    }
    auto cal = OCL::ConfigManager::FromFile(cal_file);

    try {

    } catch ( std::exception &e ){
        // Pass do nothing.
    }

    ParticleRange particleRange( options.RangeFile.value_or("") );

    std::string hist_file;
    std::string tree_file;
    if ( options.tree.value() ) {
        auto outname = options.output.value();
        outname = outname.substr(0, outname.find_last_of('.'));
        tree_file = outname + "_tree.root";
        hist_file = outname + "_hist.root";
    } else {
        hist_file = options.output.value();
    }

    Task::XIAReader reader(options.input.value(), &progress);
    Task::Calibrator calibrator(cal, reader.GetQueue());
    Task::Buffer buffer(calibrator.GetQueue());
    Task::Splitter splitter(buffer.GetQueue(), options.SplitTime.value());

    Task::Triggers triggers(splitter.GetQueue(), options.coincidenceTime.value(),
                            options.Trigger.value(), options.sortType.value());

    const char *user_sort = nullptr;
    if ( options.userSort.has_value() )
        user_sort = options.userSort->c_str();
    const char *treef = ( tree_file.empty() ) ? nullptr : tree_file.c_str();

    // For some reason the TBufferMerger consumes extreme amounts of memory.
    // I cannot see any easy way to fix it though....
    //auto merger = ( tree_file.empty() ) ? std::make_unique<ROOT::TBufferMerger>(nullptr ) : std::make_unique<ROOT::TBufferMerger>(tree_file.c_str(), "RECREATE");
    //if ( merger )
    //    merger->SetAutoSave(16384);


    Task::Sorters sorters(triggers.GetQueue(), particleRange, ( tree_file.empty() ) ? nullptr : tree_file.c_str(), user_sort);

    ThreadPool<std::thread> pool;
    pool.AddTask(&reader);
    pool.AddTask(&calibrator);
    pool.AddTask(&buffer);
    pool.AddTask(&splitter);
    pool.AddTask(triggers.GetNewTrigger());
    pool.AddTask(triggers.GetNewTrigger());

    for ( int i = 0 ; i < 4 ; ++i ){
        /*if ( merger )
            pool.AddTask(sorters.GetNewSorter(merger->GetFile()));
        else*/
            pool.AddTask(sorters.GetNewSorter());
    }

    try {
        pool.Wait();
    } catch ( const std::exception &ex ){
        std::cerr << "Got exception: " << ex.what() << std::endl;
    }

    Histograms &hm = sorters.GetHistograms();
    RootWriter::Write(hm, hist_file.c_str());
    auto root_files = sorters.GetTreeFiles();
    root_files.push_back(hist_file);
    return root_files;
}

void MergeFiles(std::string &output_file, const std::vector<std::string> &files)
{
    {
        TFileMerger merger;
        merger.OutputFile(output_file.c_str());
        for (auto &file: files) {
            merger.AddFile(file.c_str(), true);
        }
        merger.Merge();
    }
    // We can now delete all the files
    for ( auto &file : files ){
        system(std::string("rm " + file).c_str());
    }
}

int main(int argc, char *argv[])
{
    ROOT::EnableThreadSafety();
    ROOT::EnableImplicitMT();
    CLI::Options options;
    try {
        options = CLI::ParseCLA(argc, argv);
    } catch ( std::exception &e ){
        return 1; // Error
    }

    auto files = RunSort(options);
    if ( files.size() == 1 ) // Do nothing. No files to merge.
        return 0;
    else if ( files.size() > 1 )
        MergeFiles(options.output.value(), files);
    return 0;
}

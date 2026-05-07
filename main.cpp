#include <iostream>
#include <fstream>
#include <thread>
#include <signal.h>

#include "PhysicalParam/ConfigManager.h"
#include "PhysicalParam/ParticleRange.h"
#include "histogram/RootWriter.h"

#include "Tasks/Tasks.h"
#include "Tasks/XIAReader.h"
#include "Tasks/Calibrator.h"
#include "Tasks/Buffer.h"
#include "Tasks/Buffer.h"
#include "Tasks/Splitter.h"
#include "Tasks/Splitter.h"
#include "Tasks/Trigger.h"
#include "Tasks/Sort.h"
//#include "Tasks/MTSort.h"

#include "Tools/CommandLineInterface.h"
#include "Tools/ProgressUI.h"
#include "ThreadPool.hpp"

#include <TROOT.h>
#include <TFileMerger.h>

std::vector<std::string> RunSort(const CLI::Options &options, ProgressUI &progress)
{
    std::ifstream cal_file;
    try {
        cal_file = std::ifstream(options.CalibrationFile.value());
    } catch ( std::exception &e ){
        // Pass, do nothing.
        std::cerr << "Configuration file is missing." << std::endl;
        return {};
    }
    cal_file.close();
    auto cal = OCL::ConfigManager::FromFile(options.CalibrationFile.value().c_str());
    ParticleRange particleRange( options.RangeFile.value_or("") );
    auto userConfig = OCL::UserConfiguration::FromFile(options.CalibrationFile.value().c_str(),
                                                                options.Trigger.value(),
                                                                options.sortType.value(), particleRange);

    std::string hist_file;
    std::string tree_file;
    std::string conf_file;
    std::vector<std::string> root_files;
    if ( options.tree.value() ) {
        auto outname = options.output.value();
        tree_file = outname;
        hist_file = outname;
    } else {
        hist_file = options.output.value();
    }

    Task::XIAReader reader(options.input.value(), &progress);
    Task::Calibrator calibrator(cal, reader.GetQueue());
    Task::Buffer buffer(calibrator.GetQueue());
    Task::Splitter splitter(buffer.GetQueue(), options.SplitTime.value());
    Task::Trigger trigger(splitter.GetQueue(), options.coincidenceTime.value(),
                            options.Trigger.value(), options.sortType.value());


    const char *user_sort = nullptr;
    if ( options.userSort.has_value() )
        user_sort = options.userSort->c_str();

    Task::Sorter sorter(trigger.GetQueue(), userConfig,
        ( tree_file.empty() ) ? nullptr : tree_file.c_str(), options.keep_traces.value(), user_sort);

    ThreadPool<std::thread> pool;
    pool.AddTask(&reader);
    pool.AddTask(&calibrator);
    pool.AddTask(&buffer);
    pool.AddTask(&splitter);
    pool.AddTask(&trigger);
    pool.AddTask(&sorter);


    try {
        pool.Wait();
    } catch ( const std::exception &ex ){
        std::cerr << "Got exception: " << ex.what() << std::endl;
    }
    Histograms &hm = sorter.GetHistograms();
    if ( options.tree.value() ) {
        RootWriter::Write(hm, hist_file.c_str(), nullptr, "UPDATE");
    } else {
        RootWriter::Write(hm, hist_file.c_str()/*, nullptr, "UPDATE"*/);
    }

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
        std::cout << "Deleting file " << file << std::endl;
        system(std::string("rm " + file).c_str());
    }
}

int main(int argc, char *argv[])
{
    ROOT::EnableThreadSafety();
    //ROOT::EnableImplicitMT();
    CLI::Options options;
    try {
        options = CLI::ParseCLA(argc, argv);
    } catch ( std::exception &e ){
        return 1; // Error
    }
    ProgressUI progress;
    auto files = RunSort(options, progress);
    if ( files.size() == 1 ) // Do nothing. No files to merge.
        return 0;
    else if ( files.size() > 1 ) {
        auto spinner = progress.FinishSort(options.output.value());
        MergeFiles(options.output.value(), files);
        spinner.Finish();
    } else if ( files.empty() )
        return 1;
    return 0;
}

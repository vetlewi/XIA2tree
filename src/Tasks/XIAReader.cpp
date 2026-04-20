//
// Created by Vetle Wegner Ingeberg on 06/04/2022.
//

#include "XIAReader.h"

#include <memory>
#include "../../include/Tools/MemoryMap.h"

#include "Tools/ProgressUI.h"
#include "indicators/progress_spinner.hpp"

#define UPDATE_COUNT 131072 // How often should the UI be updated?

using namespace Task;

// TODO: The actual implementation of the routine that indexes the data should be refactored so that it is more testable

/*void check_consistency(const uint32_t *begin, const uint32_t *end)
{
    int scale[] = {0, 0, 8, 8, 8, 8, 8, 8, 10};
    indicators::ProgressSpinner spinner{
        indicators::option::PostfixText("Checking file consistency"),
        indicators::option::ForegroundColor{indicators::Color::yellow},
        indicators::option::SpinnerStates{std::vector<std::string>{"⠈", "⠐", "⠠", "⢀", "⡀", "⠄", "⠂", "⠁"}},
        indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}
    };

    const auto *pos = begin;
    auto pre_header = reinterpret_cast<const XIA_base_t *>(pos);
    size_t read = 0;
    while ( pos < end ){
        auto header = reinterpret_cast<const XIA_base_t *>(pos);
        if ( header->timestamp() * scale[header->slotID] < pre_header->timestamp() * scale[pre_header->slotID] ){
            std::cout << "Header timestamps: " << header->timestamp() * scale[header->slotID];
            std::cout << " " << pre_header->timestamp() * scale[header->slotID];
            std::cout << std::endl;
        }
        pre_header = header;
        pos += header->eventLen;
        // Since the if test is very rare, and very predictable this shouldn't affect the runtime that much.
        if ( read++ % (UPDATE_COUNT / 4) == 0 ){
            spinner.set_progress((pos-begin)*100.0/(end-begin));
        }

    }
    spinner.set_option(indicators::option::ForegroundColor{indicators::Color::green});
    spinner.set_option(indicators::option::PrefixText{"✔"});
    spinner.set_option(indicators::option::ShowSpinner{false});
    spinner.set_option(indicators::option::ShowPercentage{false});
    spinner.set_option(indicators::option::PostfixText{"Finished"});
    spinner.mark_as_completed();
}*/

XIAReader::XIAReader(const std::vector<std::string> &files, ProgressUI *_ui, const size_t &capacity)
    //: output_queue( capacity )
    : output_queue( )
    , ui( _ui )
{
    for ( auto &file : files ){
        mapped_files.emplace_back(file,std::make_unique<IO::MemoryMap>(file.c_str()) );
    }
}

void XIAReader::RunWithUI()
{
    QueueWorker worker(output_queue);
    int fno = 0;
    for ( const auto &[name, file] : mapped_files ){
        const auto *begin = file->GetPtr<uint32_t>();
        const auto *end = begin + file->GetSize<uint32_t>();
        const auto *pos = begin;

        auto bar = ui->StartNewFile(name, file->GetSize<uint32_t>());

        size_t read = 0;
        while ( pos < end && !done ){
            const auto *header = reinterpret_cast<const XIA_base_t *>(pos);
            if ( pos + header->eventLen <= end ) {
                output_queue.push(reinterpret_cast<const XIA_base_t *>(pos));
            }

            // Since the if test is very rare, and very predictable this shouldn't affect the runtime that much.
            if ( read++ % UPDATE_COUNT == 0 ){
                bar.UpdateProgress(pos - begin);
            }
            pos += reinterpret_cast<const XIA_base_t *>(pos)->eventLen;
        }
        bar.FinishProgress();
    }
    output_queue.mark_as_finish();
}

void XIAReader::RunWithoutUI()
{
    QueueWorker worker(output_queue);
    int fno = 0;
    for ( const auto &[name, file] : mapped_files ){
        const auto *begin = file->GetPtr<uint32_t>();
        const auto *end = begin + file->GetSize<uint32_t>();
        const auto *pos = begin;
        while ( pos < end && !done ){
            const auto *header = reinterpret_cast<const XIA_base_t *>(pos);
            if ( pos + header->eventLen <= end ) {
                output_queue.push(reinterpret_cast<const XIA_base_t *>(pos));
                pos += reinterpret_cast<const XIA_base_t *>(pos)->eventLen;
            } else {
                pos += reinterpret_cast<const XIA_base_t *>(pos)->eventLen;
            }
        }
    }
    output_queue.mark_as_finish();
}

void XIAReader::Run()
{
    if ( ui )
        RunWithUI();
    else
        RunWithoutUI();

    // Once we reach this point we are certainly done.
    is_done = true;
    done = true;
}

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

XIAReader::XIAReader(const std::vector<std::string> &files, ProgressUI *_ui)
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
            ++entries_processed;
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
             }
            pos += reinterpret_cast<const XIA_base_t *>(pos)->eventLen;
            ++entries_processed;

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

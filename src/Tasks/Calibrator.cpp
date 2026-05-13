//
// Created by Vetle Wegner Ingeberg on 06/04/2022.
//

#include "Calibrator.h"


#include <ConfigManager.h>

using namespace Task;

Calibrator::Calibrator(OCL::ConfigManager &cal, XIAQueue_t &input, const size_t &capacity)
    : calibration( cal )
    , input_queue( input )
    , output_queue( /*capacity*/ )
{
}

void Calibrator::Run()
{
    QueueWorker worker(output_queue);
    const XIA_base_t *xia;
    while ( input_queue.wait_and_pop(xia) ){
        ++entries_processed;
        
        if ( !calibration.keep(xia) ){
            continue;
        }

        Entry_t entry = calibration(xia);
        output_queue.push(entry);
    }
    output_queue.mark_as_finish();
    is_done = true;
}
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
    while ( input_queue.is_not_finish() || !input_queue.empty() ) {
        if ( !input_queue.try_pop( xia )) {
            std::this_thread::yield();
            continue;
        }

        // Get the entry. If false we can continue.
        if ( !calibration.keep(xia) ){
            continue;
        }
        Entry_t entry = calibration(xia);
        output_queue.push(entry);
    }
    output_queue.mark_as_finish();
    is_done = true;
}
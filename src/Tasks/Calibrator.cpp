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
    const XIA_base_t *xia;
    //XIA_base_t xia;
    while ( input_queue.is_not_finish() || !input_queue.empty() ) {
        if ( !input_queue.try_pop( xia )) {
            continue;
        }

        // Get the entry. If false we can continue.
        if ( !calibration.keep(xia) ){
            continue;
        }
        Entry_t entry = calibration(xia);
        output_queue.push(entry);
        /*while ( !output_queue.wait_enqueue_timed(entry, std::chrono::seconds(1)) ){
            if ( done )
                break;
        }*/
    }
    output_queue.mark_as_finish();
    /*while ( !done ){
        while ( !input_queue.wait_dequeue_timed(xia, std::chrono::seconds(1)) ){
            if ( done )
                break;
        }

        // Get the entry. If false we can continue.
        if ( !calibration.keep(xia) ){
            continue;
        }
        Entry_t entry = calibration(xia);
        while ( !output_queue.wait_enqueue_timed(entry, std::chrono::seconds(1)) ){
            if ( done )
                break;
        }
    }*/
    is_done = true;
}
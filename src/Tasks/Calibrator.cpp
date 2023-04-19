//
// Created by Vetle Wegner Ingeberg on 06/04/2022.
//

#include "Calibrator.h"



//#include "PhysicalParam/Calibration.h"


using namespace Task;

Calibrator::Calibrator(OCL::ConfigManager &cal, XIAQueue_t &input, const size_t &capacity)
    : calibration( cal )
    , input_queue( input )
    , output_queue( capacity )
{
}

void Calibrator::Run()
{
    const XIA_base_t *xia;
    //XIA_base_t xia;
    while ( !done ){
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
    }
    is_done = true;
}
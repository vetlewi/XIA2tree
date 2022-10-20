//
// Created by Vetle Wegner Ingeberg on 06/04/2022.
//

#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include <Task.h>
#include <Queue.h>
#include "PhysicalParam/Calibration.h"


namespace Task {

    class Calibrator : public Base {
    private:

        Calibration &calibration;
        EntryQueue_t output_queue;
        XIAQueue_t &input_queue;

    public:
        Calibrator(Calibration &cal, XIAQueue_t &input, const size_t &capacity = 1024);

        auto &GetQueue(){ return output_queue; }

        void Run() override;

    };

}

#endif // CALIBRATOR_H

//
// Created by Vetle Wegner Ingeberg on 16/04/2021.
//

#ifndef TDR2TREE_TRIGGER_H
#define TDR2TREE_TRIGGER_H

#include <Tasks.h>
#include <Queue.h>
#include <experimentsetup.h>

namespace Task {

    class Trigger : public Base
    {
    private:
        EventQueue_t &input_queue;
        TEventQueue_t output_queue;

        const double coincidence_time;
        const DetectorType trigger;
        const bool time_cal;

    public:
        Trigger(EventQueue_t &input, const double &time = 1500., const DetectorType &trigger = DetectorType::eDet, const bool &time_cal = false,
                const size_t &cap = 65536);
        TEventQueue_t &GetQueue(){ return output_queue; }

        void Run() override;

    };

}

#endif //TDR2TREE_TRIGGER_H

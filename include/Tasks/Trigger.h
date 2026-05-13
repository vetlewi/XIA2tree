//
// Created by Vetle Wegner Ingeberg on 16/04/2021.
//

#ifndef TDR2TREE_TRIGGER_H
#define TDR2TREE_TRIGGER_H

#include <Tasks.h>
#include <Queue.h>
#include <CommandLineInterface.h>

namespace Task {

    class Trigger : public Base
    {
    private:
        MCEventQueue_t &input_queue;
        MTEventQueue_t output_queue;

        const double coincidence_time;
        const DetectorType trigger;
        const CLI::sort_type sort_type;

    public:
        CLASS_NAME(Trigger)
        Trigger(MCEventQueue_t &input, const double &time = 1500., const DetectorType &trigger = DetectorType::eDet,
                const CLI::sort_type &sort_type = CLI::sort_type::coincidence);
        MTEventQueue_t &GetQueue(){ return output_queue; }

        void Run() override;

    };
}

#endif //TDR2TREE_TRIGGER_H

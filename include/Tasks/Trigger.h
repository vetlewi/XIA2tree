//
// Created by Vetle Wegner Ingeberg on 16/04/2021.
//

#ifndef TDR2TREE_TRIGGER_H
#define TDR2TREE_TRIGGER_H

#include <Tasks.h>
#include <Queue.h>
#include <experimentsetup.h>
#include <CommandLineInterface.h>

namespace Task {

    class Trigger : public Base
    {
    private:
        MCEventQueue_t &input_queue;
        TEventQueue_t output_queue;

        const double coincidence_time;
        const DetectorType trigger;
        const bool time_cal;

    public:
        Trigger(MCEventQueue_t &input, const double &time = 1500., const DetectorType &trigger = DetectorType::eDet, const bool &time_cal = false,
                const size_t &cap = 65536);
        TEventQueue_t &GetQueue(){ return output_queue; }

        void Run() override;

    };

    class STrigger : public Base
    {
    private:
        MCEventQueue_t &input_queue;
        TEventQueue_t &output_queue;

        const double coincidence_time;
        const DetectorType trigger;
        const CLI::sort_type sort_type;

    public:
        STrigger(MCEventQueue_t &input, TEventQueue_t &output,
                const double &time = 1500., const DetectorType &trigger = DetectorType::eDet,
                const CLI::sort_type &sort_type = CLI::sort_type::coincidence);

        void Run() override;

    };

    class Triggers
    {
    private:
        MCEventQueue_t &input_queue;
        TEventQueue_t output_queue;

        const double coincidence_time;
        const DetectorType trigger;
        const CLI::sort_type sort_type;

        std::vector<STrigger *> triggers;

    public:
        Triggers(MCEventQueue_t &input, const double &time = 1500., const DetectorType &trigger = DetectorType::eDet,
                 const CLI::sort_type &sort_type = CLI::sort_type::coincidence, const size_t &cap = 65536);
        ~Triggers();
        TEventQueue_t &GetQueue(){ return output_queue; }

        STrigger *GetNewTrigger();

    };

}

#endif //TDR2TREE_TRIGGER_H

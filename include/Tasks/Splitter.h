//
// Created by Vetle Wegner Ingeberg on 16/04/2021.
//

#ifndef TDR2TREE_SPLITTER_H
#define TDR2TREE_SPLITTER_H

#include <Tasks.h>
#include <Queue.h>

namespace Task {

    class Splitter : public Base
    {
    private:
        EventQueue_t &input_queue;
        MCEventQueue_t output_queue;
        const double gap;
        //std::vector<word_t> buffer;

        void SplitEntries();

    public:

        Splitter(EventQueue_t &input, const double &time_gap = 1500., const size_t &cap = 65536);
        MCEventQueue_t &GetQueue(){ return output_queue; }
        void Run() override;

    };

}

#endif //TDR2TREE_SPLITTER_H

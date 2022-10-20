//
// Created by Vetle Wegner Ingeberg on 16/04/2021.
//

#include "Splitter.h"
#include "timeval.h"

using namespace Task;


Splitter::Splitter(EventQueue_t &input, const double &time_gap, const size_t &cap)
    : input_queue( input ), output_queue( cap ), gap( time_gap ){}

template<typename T>
T Split(T begin, T end, const double gap)
{
    return std::adjacent_find(begin, end,
                                  [gap](const Entry_t &lhs, const Entry_t &rhs){
        return  abs((double(rhs.timestamp - lhs.timestamp) + (rhs.cfdcorr - lhs.cfdcorr))) > gap;
    }) + 1;
}

void Splitter::Run()
{
    std::vector<Entry_t> entries;
    while ( !done && input_queue.wait_dequeue_timed(entries, std::chrono::seconds(1)) ){

        // Continue pushing to the output queue until all entries has been split.
        auto entries_begin = entries.begin();
        auto entries_end = entries.begin();
        while ( entries_end < entries.end() ){
            entries_end = Split(entries_begin, entries.end(), gap);
            while ( !output_queue.try_enqueue(std::vector(entries_begin, entries_end)) ){
                if ( done )
                    break;
            }
            entries_begin = entries_end;
        }
    }
    is_done = true;
}
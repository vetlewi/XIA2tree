//
// Created by Vetle Wegner Ingeberg on 16/04/2021.
//

#include "Trigger.h"

#include <stdexcept>

using namespace Task;

Trigger::Trigger(EventQueue_t &input, const double &time, const DetectorType &trig, const bool &_tcal, const size_t &cap)
    : input_queue( input )
    , output_queue( cap )
    , coincidence_time( time )
    , trigger( trig )
    , time_cal( _tcal )
{}

std::vector<Entry_t>::iterator find_start(const std::vector<Entry_t>::iterator &begin, const std::vector<Entry_t>::iterator &end, const double &ctime)
{
    // Both T are forward iterators, but end should be smaller than begin.
    auto pos = begin - 1;
    while ( pos >= end ){
        if ( abs((double(pos->timestamp - begin->timestamp) + (pos->cfdcorr - begin->cfdcorr))) > ctime ){
            return pos;
        }
        --pos;
    }
    return end;
}

std::vector<Entry_t>::iterator find_end(const std::vector<Entry_t>::iterator &begin, const std::vector<Entry_t>::iterator &end, const double &ctime)
{
    // Both T are forward iterators, but end should be smaller than begin.
    auto pos = begin;
    while ( pos < end ){
        if ( abs((double(pos->timestamp - begin->timestamp) + (pos->cfdcorr - begin->cfdcorr))) > ctime ){
            return pos + 1;
        }
        ++pos;
    }
    return end;
}

template<typename T>
std::vector<typename T::const_iterator> GetTriggers(const T &entries, const DetectorType &trigger)
{
    std::vector<typename T::const_iterator> triggers;
    for ( auto it = entries.begin() ; it != entries.end() ; ++it ){
        if ( it->type == trigger ){
            triggers.push_back(it);
        }
    }
    return triggers;
}

void Trigger::Run()
{
    std::vector<Entry_t> input;

    while ( !done ){
        if ( input_queue.wait_dequeue_timed(input, std::chrono::seconds(1)) ){

            // First we will find all the entries that corresponds to a "correct" trigger within the buffer
            auto triggers = GetTriggers(input, trigger);

            for ( auto &trig : triggers ){

                if ( time_cal ){ // If it is a time calibration run, we only care about the timing relative to the "trigger"
                    if ( trig->detectorID != 0 )
                        continue;
                }

                auto begin = trig;
                for ( begin = trig ; begin > input.begin() ; --begin ){
                    if ( abs( ((begin-1)->timestamp - trig->timestamp) +
                                      ((begin-1)->cfdcorr - trig->cfdcorr) ) > coincidence_time )
                        break;
                }

                auto end = trig + 1;
                for ( end = trig + 1 ; end != input.end() ; ++end ){
                    if ( abs( (end->timestamp - trig->timestamp) +
                              (end->cfdcorr - trig->cfdcorr) ) > coincidence_time )
                        break;
                }

                auto evt = std::make_pair(std::vector(begin, end), trig - begin);
                while ( !output_queue.try_enqueue(evt) ){
                    if ( done )
                        break;
                }
                /*while ( !output_queue.wait_enqueue_timed(std::make_pair(std::vector(begin, end), trig - begin),
                                                         std::chrono::seconds(1)) ){
                    if ( done )
                        break;
                }*/

            }
        }
    }
    is_done = true;
}
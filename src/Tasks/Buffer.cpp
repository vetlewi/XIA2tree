//
// Created by Vetle Wegner Ingeberg on 16/04/2021.
//

#include "Buffer.h"

using namespace Task;

Buffer::Buffer(EntryQueue_t &input, const size_t &buf_size, const size_t &cap)
    : input_queue( input )
    , output_queue( /*cap*/ )
    , size( buf_size )
{
    buffer.reserve( 2*size );
}

void Buffer::Run()
{
    Task::QueueWorker worker(output_queue);
    Entry_t event;
    while ( input_queue.is_not_finish() || !input_queue.empty() ) {
        if ( buffer.size() > size ){
            std::sort(buffer.begin(), buffer.end(), [](const Entry_t &lhs, const Entry_t &rhs)
            { return (double(rhs.timestamp - lhs.timestamp) + (rhs.cfdcorr - lhs.cfdcorr)) > 0; });

            auto end = std::adjacent_find(buffer.begin()+size, buffer.end(), [](const Entry_t &lhs, const Entry_t &rhs)
            { return  (double(rhs.timestamp - lhs.timestamp) + (rhs.cfdcorr - lhs.cfdcorr)) > 50000; });

            //output_queue.wait_enqueue(std::vector(buffer.begin(), end));
            output_queue.push(std::vector(buffer.begin(), end));
            buffer.erase(buffer.begin(), end);
        }
        if ( !input_queue.try_pop( event )) {
            continue;
        }
        buffer.push_back(event);
    }

    // We reach this point if we are told we are done. Flush buffer.
    output_queue.push(buffer);
    is_done = true;
    output_queue.mark_as_finish();
}